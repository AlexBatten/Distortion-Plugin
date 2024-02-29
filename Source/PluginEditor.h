/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class DistortionPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DistortionPluginAudioProcessorEditor (DistortionPluginAudioProcessor&);
    ~DistortionPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    Slider distortionKnob;
    ToggleButton bitCrushButton;
    Slider bitCrushDepthKnob;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> distortionKnobAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bitCrushButtonAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> bitCrushDepthKnobAttachment;

    Image background;

    DistortionPluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionPluginAudioProcessorEditor)
};
