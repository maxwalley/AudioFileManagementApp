#pragma once

#include <JuceHeader.h>
#include "FXAndCategoryBrowser.h"
#include "AudioPlayerComponent.h"

//==============================================================================
class MainComponent  : public Component,
                       public Slider::Listener
{
public:
    //==============================================================================
    MainComponent(const ValueTree& dataToDisplay, DataTreeManager& treeManager);
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    void sliderValueChanged(Slider* slider) override;
    
    FXAndCategoryBrowser browser;
    AudioPlayerComponent player;
    
    Slider itemSizeSlider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
