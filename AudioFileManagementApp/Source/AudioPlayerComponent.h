/*
  ==============================================================================

    AudioPlayer.h
    Created: 30 Oct 2020 3:34:01pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class AudioPlayerComponent  : public juce::Component
{
public:
    AudioPlayerComponent();
    ~AudioPlayerComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayerComponent)
};
