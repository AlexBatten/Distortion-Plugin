/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

//==============================================================================
DistortionPluginAudioProcessorEditor::DistortionPluginAudioProcessorEditor (DistortionPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    
    background = juce::ImageCache::getFromMemory(BinaryData::Abstractart_png, BinaryData::Abstractart_pngSize);

    distortionKnob.setSliderStyle(Slider::Rotary);
    distortionKnob.setRange(0.0, 100.0, 1.0);
    addAndMakeVisible(&distortionKnob);

    addAndMakeVisible(bitCrushButton);



    distortionKnobAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "distortionAmount", distortionKnob);

    bitCrushButtonAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "bitCrushEnabled", bitCrushButton);



    setSize (400, 300);
}

DistortionPluginAudioProcessorEditor::~DistortionPluginAudioProcessorEditor()
{
}

//==============================================================================
void DistortionPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    /*g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));*/

    // Draw the background image
    if (!background.isNull())
        g.drawImageAt(background, 0, 0);
    else
        g.fillAll(juce::Colours::black);  // Fallback background color

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Distortion Effect", getLocalBounds(), juce::Justification::centred, 1);
}

void DistortionPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    //distortionKnob.setBounds(getLocalBounds().reduced(40));

    // Example values for the slider's position and size
    int sliderX = 50;
    int sliderY = 50;
    int sliderWidth = 200;
    int sliderHeight = 40;

    // Set the bounds for the distortion slider
    distortionKnob.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);

    // Set the bounds for the bit crush button
    // Positioning it below the slider with a padding of 10 pixels
    int buttonPadding = 10;
    int buttonWidth = 100;
    int buttonHeight = 30;
    bitCrushButton.setBounds(sliderX, sliderY + sliderHeight + buttonPadding, buttonWidth, buttonHeight);

}
