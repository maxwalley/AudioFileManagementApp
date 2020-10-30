#pragma once

#include <JuceHeader.h>
#include "FXAndCategoryBrowser.h"
#include "AudioPlayer.h"

//==============================================================================
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent(const ValueTree& dataToDisplay, DataTreeManager& treeManager);
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void refreshBrowser();

private:

    FXAndCategoryBrowser browser;
    AudioPlayer player;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
