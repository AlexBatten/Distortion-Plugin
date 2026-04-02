/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DistortionPluginAudioProcessorEditor::DistortionPluginAudioProcessorEditor (DistortionPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    auto accentColour = juce::Colour (0xFFE04040);

    // Title label
    titleLabel.setText ("BIT COLLAPSE", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (juce::FontOptions (22.0f).withStyle ("Bold")));
    titleLabel.setColour (juce::Label::textColourId, accentColour);
    titleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel);

    // Distortion knob
    distortionKnob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    distortionKnob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    distortionKnob.setColour (juce::Slider::rotarySliderFillColourId, accentColour);
    distortionKnob.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xFF404040));
    distortionKnob.setColour (juce::Slider::thumbColourId, accentColour);
    distortionKnob.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    distortionKnob.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xFF404040));
    addAndMakeVisible (distortionKnob);

    // Distortion label
    distortionLabel.setText ("DISTORTION", juce::dontSendNotification);
    distortionLabel.setFont (juce::Font (juce::FontOptions (13.0f)));
    distortionLabel.setColour (juce::Label::textColourId, juce::Colours::grey);
    distortionLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (distortionLabel);

    // Bit crush toggle
    bitCrushButton.setButtonText ("BIT CRUSH");
    bitCrushButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    bitCrushButton.setColour (juce::ToggleButton::tickColourId, accentColour);
    bitCrushButton.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xFF404040));
    addAndMakeVisible (bitCrushButton);

    // Parameter attachments
    distortionKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "distortionAmount", distortionKnob);

    bitCrushButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.parameters, "bitCrushEnabled", bitCrushButton);

    setSize (300, 340);
}

DistortionPluginAudioProcessorEditor::~DistortionPluginAudioProcessorEditor()
{
}

//==============================================================================
void DistortionPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Dark background
    g.fillAll (juce::Colour (0xFF1A1A1A));

    // Subtle border
    g.setColour (juce::Colour (0xFF303030));
    g.drawRect (getLocalBounds(), 1);

    // Separator line below title
    int separatorY = 50;
    g.setColour (juce::Colour (0xFF303030));
    g.drawHorizontalLine (separatorY, 10.0f, (float) getWidth() - 10.0f);
}

void DistortionPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    titleLabel.setBounds (area.removeFromTop (48));

    auto knobArea = area.removeFromTop (200);
    distortionKnob.setBounds (knobArea.reduced (50, 10));

    distortionLabel.setBounds (area.removeFromTop (20));

    auto buttonArea = area.removeFromTop (40);
    bitCrushButton.setBounds (buttonArea.withSizeKeepingCentre (140, 30));
}
