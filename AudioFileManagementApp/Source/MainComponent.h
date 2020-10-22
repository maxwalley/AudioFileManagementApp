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
    
    ValueTree getDisplayedTree() const;
    
private:
    bool canBeOpened() override;
    
    void paint(Graphics& g) override;
    void openessChanged(bool newState) override;
    void itemDroppedIntoThisItem(ThumbnailBrowserItem* droppedItem) override;
    
    void valueTreeChildAdded(ValueTree& parent, ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(ValueTree& parent, ValueTree& removedChild, int index) override;
    
    //fxToLookUp should be from the FXList of a category
    ValueTree lookUpAndFindFX(const ValueTree& fxToLookUp);
    
    ValueTree displayedTree;
    
    DataTreeManager& treeManager;
};

//==============================================================================

//Browser with some custom mouse controls
class FXAndCategoryBrowser  : public HierarchicalThumbnailBrowser,
                              public FileDragAndDropTarget
{
public:
    FXAndCategoryBrowser();
    ~FXAndCategoryBrowser();
    
private:
    void paintOverChildren(Graphics& g) override;
    
    void mouseDown(const MouseEvent& event) override;
    
    bool isInterestedInFileDrag(const StringArray& files) override;
    void fileDragMove(const StringArray& files, int x, int y) override;
    void fileDragExit(const StringArray& files) override;
    void filesDropped(const StringArray &files, int x, int y) override;
    
    void addCategoryToDisplayedTree();
    
    bool filesBeingDragged = false;
    
};

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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
