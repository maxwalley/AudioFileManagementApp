/*
  ==============================================================================

    AudioPlayer.cpp
    Created: 30 Oct 2020 3:34:01pm
    Author:  Max Walley

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioPlayerComponent.h"

//==============================================================================
AudioPlayerComponent::AudioPlayerComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

AudioPlayerComponent::~AudioPlayerComponent()
{
}

void AudioPlayerComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::green);
}

void AudioPlayerComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
