#pragma once

#include <JuceHeader.h>
#include "HierarchicalThumbnailBrowser.h"

//==============================================================================

class FXAndCategoryBrowserItem  : ThumbnailBrowserItem
{
public:
    FXAndCategoryBrowserItem(const ValueTree& treeToDisplay);
    ~FXAndCategoryBrowserItem();
    
private:
    
    bool canBeOpened() override;
    
    void paintItem(Graphics& g, int width, int height) override;
    
    ValueTree displayedTree;
};

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
