#pragma once

#include <JuceHeader.h>
#include "HierarchicalThumbnailBrowser.h"

//==============================================================================

class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent(const ValueTree& dataToDisplay);
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    HierarchicalThumbnailBrowser browser;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
