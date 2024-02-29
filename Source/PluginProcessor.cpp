/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

/* this is where variables for audio processing will be stored */

static AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>("distortionAmount",
        "Distortion",
        NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        1.0f));

    params.push_back(std::make_unique<AudioParameterBool>("bitCrushEnabled",
        "Bit Crush Enabled",
        false));

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
    parameters(*this, nullptr, "Parameters", createParameterLayout())
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void DistortionPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DistortionPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Retrieve the clean signal
            float cleanSignal = channelData[sample];

            float currentDistortionAmount = *parameters.getRawParameterValue("distortionAmount");

            bool bitCrushEnabled = *parameters.getRawParameterValue("bitCrushEnabled");


            // Apply distortion based on the currentDistortionAmount
            if (currentDistortionAmount > 0) {
                // Scale the distortion effect
                float scaledAmount = currentDistortionAmount / 10.0f; // Scaling to a 0.0 - 1.0 range

                float bitCrushedSignal = applyBitCrush(cleanSignal, bitCrushEnabled);

                float distortedSignal = applyDistortion(bitCrushedSignal, scaledAmount);

                // Mix clean and distorted signal based on the amount
                channelData[sample] = cleanSignal * (1.0f - scaledAmount) + distortedSignal * scaledAmount;

                

                // LIMIT 0DB
                channelData[sample] = std::clamp(channelData[sample], -1.0f, 1.0f);
            }
            else {
                // No distortion, pass the clean signal
                channelData[sample] = cleanSignal;
            }
        }

    }
}

float DistortionPluginAudioProcessor::applyDistortion(float inputSignal, float amount)
{
    // Your distortion algorithm here, for example, simple clipping
    float distortedSignal = inputSignal * (1.0f + amount);
    return (distortedSignal > 1.0f) ? 1.0f : (distortedSignal < -1.0f ? -1.0f : distortedSignal);
}

float DistortionPluginAudioProcessor::applyBitCrush(float inputSignal, bool bitCrushEnabled)
{
    if (!bitCrushEnabled) return inputSignal;

    constexpr int bitDepth = 8;  // You can make this a parameter too
    const float scaleFactor = static_cast<float>(1 << (bitDepth - 1));
    return std::floor(inputSignal * scaleFactor) / scaleFactor;
}


//==============================================================================
bool DistortionPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DistortionPluginAudioProcessor::createEditor()
{
    return new DistortionPluginAudioProcessorEditor (*this);
}

//==============================================================================
void DistortionPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

     // Create an XML element with the state of the APVTS
    std::unique_ptr<XmlElement> xml(parameters.state.createXml());

    // Store this XML in the provided memory block
    if (xml != nullptr)
        copyXmlToBinary(*xml, destData);

}

void DistortionPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

     // Create an XML element from the saved data
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    // If this XML is valid, use it to set the state of the APVTS
    if (xmlState != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.state = ValueTree::fromXml(*xmlState);

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DistortionPluginAudioProcessor();
}
