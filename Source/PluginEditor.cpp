/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

static void setupRotaryKnob (juce::Slider& knob, juce::Colour accent)
{
    knob.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    knob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 50, 18);
    knob.setColour (juce::Slider::rotarySliderFillColourId, accent);
    knob.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xFF404040));
    knob.setColour (juce::Slider::thumbColourId, accent);
    knob.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    knob.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

static void setupLabel (juce::Label& label, const juce::String& text)
{
    label.setText (text, juce::dontSendNotification);
    label.setFont (juce::Font (juce::FontOptions (11.0f)));
    label.setColour (juce::Label::textColourId, juce::Colour (0xFF888888));
    label.setJustificationType (juce::Justification::centred);
}

//==============================================================================
DistortionPluginAudioProcessorEditor::DistortionPluginAudioProcessorEditor (DistortionPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    auto red    = juce::Colour (0xFFE04040);
    auto blue   = juce::Colour (0xFF4090E0);
    auto orange = juce::Colour (0xFFE0A040);
    auto green  = juce::Colour (0xFF40C070);

    // Title
    titleLabel.setText ("BIT COLLAPSE", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (juce::FontOptions (24.0f).withStyle ("Bold")));
    titleLabel.setColour (juce::Label::textColourId, red);
    titleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel);

    // Distortion type selector
    distortionTypeBox.addItem ("Hard Clip", 1);
    distortionTypeBox.addItem ("Soft Clip", 2);
    distortionTypeBox.addItem ("Foldback", 3);
    distortionTypeBox.addItem ("Arc Tan", 4);
    distortionTypeBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xFF2A2A2A));
    distortionTypeBox.setColour (juce::ComboBox::outlineColourId, juce::Colour (0xFF404040));
    distortionTypeBox.setColour (juce::ComboBox::textColourId, juce::Colours::white);
    addAndMakeVisible (distortionTypeBox);
    setupLabel (distortionTypeLabel, "TYPE");
    addAndMakeVisible (distortionTypeLabel);

    // Drive / Mix / Tone knobs
    setupRotaryKnob (driveKnob, red);
    addAndMakeVisible (driveKnob);
    setupLabel (driveLabel, "DRIVE");
    addAndMakeVisible (driveLabel);

    setupRotaryKnob (mixKnob, blue);
    addAndMakeVisible (mixKnob);
    setupLabel (mixLabel, "MIX");
    addAndMakeVisible (mixLabel);

    setupRotaryKnob (toneKnob, orange);
    addAndMakeVisible (toneKnob);
    setupLabel (toneLabel, "TONE");
    addAndMakeVisible (toneLabel);

    // Bit crush toggle
    bitCrushButton.setButtonText ("BIT CRUSH");
    bitCrushButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    bitCrushButton.setColour (juce::ToggleButton::tickColourId, green);
    bitCrushButton.setColour (juce::ToggleButton::tickDisabledColourId, juce::Colour (0xFF404040));
    addAndMakeVisible (bitCrushButton);

    // Bit crush knobs
    setupRotaryKnob (bitDepthKnob, green);
    bitDepthKnob.setTextValueSuffix (" bit");
    addAndMakeVisible (bitDepthKnob);
    setupLabel (bitDepthLabel, "DEPTH");
    addAndMakeVisible (bitDepthLabel);

    setupRotaryKnob (downsampleKnob, green);
    downsampleKnob.setTextValueSuffix ("x");
    addAndMakeVisible (downsampleKnob);
    setupLabel (downsampleLabel, "DOWNSAMPLE");
    addAndMakeVisible (downsampleLabel);

    // Parameter attachments
    distortionTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        audioProcessor.parameters, "distortionType", distortionTypeBox);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "drive", driveKnob);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "mix", mixKnob);
    toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "tone", toneKnob);
    bitCrushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        audioProcessor.parameters, "bitCrushEnabled", bitCrushButton);
    bitDepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "bitDepth", bitDepthKnob);
    downsampleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        audioProcessor.parameters, "downsample", downsampleKnob);
    setSize (420, 500);
}

DistortionPluginAudioProcessorEditor::~DistortionPluginAudioProcessorEditor()
{
}

//==============================================================================
void DistortionPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xFF1A1A1A));

    // Border
    g.setColour (juce::Colour (0xFF303030));
    g.drawRect (getLocalBounds(), 1);

    // Separator below title
    g.drawHorizontalLine (52, 10.0f, (float) getWidth() - 10.0f);

    // Separator between distortion and bit crush sections
    int crushSectionY = 280;
    g.drawHorizontalLine (crushSectionY, 10.0f, (float) getWidth() - 10.0f);

    // "BIT CRUSH" section header background
    g.setColour (juce::Colour (0xFF222222));
    g.fillRect (0, crushSectionY + 1, getWidth(), 34);
}

void DistortionPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Title
    titleLabel.setBounds (area.removeFromTop (50));
    area.removeFromTop (8);

    // Distortion type selector row
    auto typeRow = area.removeFromTop (30);
    distortionTypeLabel.setBounds (typeRow.removeFromLeft (50));
    distortionTypeBox.setBounds (typeRow.reduced (10, 0));

    area.removeFromTop (10);

    // Drive / Mix / Tone knobs
    auto knobRow = area.removeFromTop (140);
    int knobWidth = knobRow.getWidth() / 3;

    auto driveArea = knobRow.removeFromLeft (knobWidth);
    driveKnob.setBounds (driveArea.removeFromTop (120).reduced (10, 0));
    driveLabel.setBounds (driveArea);

    auto mixArea = knobRow.removeFromLeft (knobWidth);
    mixKnob.setBounds (mixArea.removeFromTop (120).reduced (10, 0));
    mixLabel.setBounds (mixArea);

    auto toneArea = knobRow;
    toneKnob.setBounds (toneArea.removeFromTop (120).reduced (10, 0));
    toneLabel.setBounds (toneArea);

    // Bit crush section
    area.removeFromTop (42); // separator gap

    // Bit crush toggle
    auto toggleRow = area.removeFromTop (34);
    bitCrushButton.setBounds (toggleRow.withSizeKeepingCentre (140, 30));

    area.removeFromTop (6);

    // Bit Depth / Downsample knobs (centered)
    auto crushKnobRow = area.removeFromTop (140);
    int crushKnobWidth = crushKnobRow.getWidth() / 2;

    auto depthArea = crushKnobRow.removeFromLeft (crushKnobWidth);
    bitDepthKnob.setBounds (depthArea.removeFromTop (120).reduced (30, 0));
    bitDepthLabel.setBounds (depthArea);

    auto dsArea = crushKnobRow;
    downsampleKnob.setBounds (dsArea.removeFromTop (120).reduced (30, 0));
    downsampleLabel.setBounds (dsArea);
}
