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

    juce::Label titleLabel;

    // Distortion controls
    juce::ComboBox distortionTypeBox;
    juce::Label distortionTypeLabel;

    juce::Slider driveKnob;
    juce::Label driveLabel;

    juce::Slider mixKnob;
    juce::Label mixLabel;

    juce::Slider toneKnob;
    juce::Label toneLabel;

    // Bit crush controls
    juce::ToggleButton bitCrushButton;

    juce::Slider bitDepthKnob;
    juce::Label bitDepthLabel;

    juce::Slider downsampleKnob;
    juce::Label downsampleLabel;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distortionTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bitCrushAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitDepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> downsampleAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DistortionPluginAudioProcessorEditor)
};
