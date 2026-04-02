/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<AudioParameterChoice> (
        "distortionType", "Distortion Type",
        juce::StringArray { "Hard Clip", "Soft Clip", "Foldback", "Arc Tan" }, 0));

    params.push_back (std::make_unique<AudioParameterFloat> (
        "drive", "Drive",
        NormalisableRange<float> (0.0f, 100.0f, 0.1f), 20.0f));

    params.push_back (std::make_unique<AudioParameterFloat> (
        "mix", "Mix",
        NormalisableRange<float> (0.0f, 100.0f, 0.1f), 50.0f));

    params.push_back (std::make_unique<AudioParameterFloat> (
        "tone", "Tone",
        NormalisableRange<float> (0.0f, 100.0f, 0.1f), 100.0f));

    params.push_back (std::make_unique<AudioParameterBool> (
        "bitCrushEnabled", "Bit Crush Enabled", false));

    params.push_back (std::make_unique<AudioParameterFloat> (
        "bitDepth", "Bit Depth",
        NormalisableRange<float> (1.0f, 16.0f, 1.0f), 8.0f));

    params.push_back (std::make_unique<AudioParameterFloat> (
        "downsample", "Downsample",
        NormalisableRange<float> (1.0f, 50.0f, 1.0f), 1.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
DistortionPluginAudioProcessor::DistortionPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    parameters (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

DistortionPluginAudioProcessor::~DistortionPluginAudioProcessor()
{
}

//==============================================================================
const juce::String DistortionPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DistortionPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DistortionPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DistortionPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DistortionPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DistortionPluginAudioProcessor::getNumPrograms()
{
    return 1;
}

int DistortionPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DistortionPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DistortionPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void DistortionPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DistortionPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    toneFilterState[0] = 0.0f;
    toneFilterState[1] = 0.0f;
    downsampleHold[0] = 0.0f;
    downsampleHold[1] = 0.0f;
    downsampleCounter[0] = 0;
    downsampleCounter[1] = 0;
}

void DistortionPluginAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DistortionPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Read parameters
    int distType = (int) *parameters.getRawParameterValue ("distortionType");
    float drive = *parameters.getRawParameterValue ("drive") / 100.0f;
    float mix = *parameters.getRawParameterValue ("mix") / 100.0f;
    float toneParam = *parameters.getRawParameterValue ("tone") / 100.0f;
    bool bitCrush = *parameters.getRawParameterValue ("bitCrushEnabled") > 0.5f;
    int bitDepth = (int) *parameters.getRawParameterValue ("bitDepth");
    int downsampleFactor = (int) *parameters.getRawParameterValue ("downsample");

    float toneCoeff = 0.005f + toneParam * 0.995f;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float cleanSignal = channelData[sample];
            float processed = cleanSignal;

            // Bit crush (pre-distortion)
            if (bitCrush)
            {
                // Downsample: sample-and-hold at reduced rate
                if (++downsampleCounter[channel] >= downsampleFactor)
                {
                    downsampleCounter[channel] = 0;
                    downsampleHold[channel] = processed;
                }
                processed = downsampleHold[channel];

                // Bit depth reduction
                float scaleFactor = static_cast<float> (1 << (bitDepth - 1));
                processed = std::floor (processed * scaleFactor) / scaleFactor;
            }

            // Apply distortion
            processed = applyDistortion (processed, drive, distType);

            // Tone filter (one-pole low-pass)
            toneFilterState[channel] += toneCoeff * (processed - toneFilterState[channel]);
            processed = toneFilterState[channel];

            // Wet/dry mix
            channelData[sample] = cleanSignal * (1.0f - mix) + processed * mix;

            // Hard limit
            channelData[sample] = std::clamp (channelData[sample], -1.0f, 1.0f);
        }
    }
}

float DistortionPluginAudioProcessor::applyDistortion (float sample, float drive, int type)
{
    // Scale drive: 0..1 mapped to 1..51 for gain staging
    float gain = 1.0f + drive * 50.0f;
    float driven = sample * gain;

    switch (type)
    {
        case HardClip:
            return std::clamp (driven, -1.0f, 1.0f);

        case SoftClip:
            return std::tanh (driven);

        case Foldback:
        {
            // Foldback distortion: signal wraps when exceeding threshold
            float threshold = 1.0f;
            while (driven > threshold || driven < -threshold)
            {
                if (driven > threshold)
                    driven = 2.0f * threshold - driven;
                else if (driven < -threshold)
                    driven = -2.0f * threshold - driven;
            }
            return driven;
        }

        case ArcTan:
            return (2.0f / juce::MathConstants<float>::pi) * std::atan (driven);

        default:
            return std::clamp (driven, -1.0f, 1.0f);
    }
}

//==============================================================================
bool DistortionPluginAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* DistortionPluginAudioProcessor::createEditor()
{
    return new DistortionPluginAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml (parameters.state.createXml());
    if (xml != nullptr)
        copyXmlToBinary (*xml, destData);
}

void DistortionPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.state = ValueTree::fromXml (*xmlState);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionPluginAudioProcessor();
}
