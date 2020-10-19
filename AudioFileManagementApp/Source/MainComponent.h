#pragma once

#include <JuceHeader.h>
#include "HierarchicalThumbnailBrowser.h"
#include "DataTreeManager.h"

//==============================================================================

class FXAndCategoryBrowserItem  : public ThumbnailBrowserItem,
                                  public ValueTree::Listener
{
public:
    FXAndCategoryBrowserItem(const ValueTree& treeToDisplay, DataTreeManager& dataManager);
    ~FXAndCategoryBrowserItem();
    
private:
    bool canBeOpened() override;
    
    void paint(Graphics& g) override;
    void openessChanged(bool newState) override;
    
    void valueTreeChildAdded(ValueTree& parent, ValueTree& childWhichHasBeenAdded) override;
    
    //fxToLookUp should be from the FXList of a category
    void lookUpAndAddFXAsChild(const ValueTree& fxToLookUp);
    
    ValueTree displayedTree;
    
    DataTreeManager& treeManager;
};

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

    HierarchicalThumbnailBrowser browser;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
