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
    DistortionPluginAudioProcessor& audioProcessor;

    juce::Slider distortionKnob;
    juce::ToggleButton bitCrushButton;
    juce::Label distortionLabel;
    juce::Label titleLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionKnobAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bitCrushButtonAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionPluginAudioProcessorEditor)
};
