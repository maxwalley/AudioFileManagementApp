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
    
}
