/*
  ==============================================================================

    FXAndCategoryBrowser.h
    Created: 30 Oct 2020 2:53:16pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include "HierarchicalThumbnailBrowser.h"
#include "DataTreeManager.h"

class FXAndCategoryBrowserItem  : public ThumbnailBrowserItem,
                                  public ValueTree::Listener
{
public:
    FXAndCategoryBrowserItem(const ValueTree& treeToDisplay, DataTreeManager& dataManager);
    ~FXAndCategoryBrowserItem();
    
    ValueTree getDisplayedTree() const;
    
protected:
    virtual void paintOverlay(Graphics& g){};
    
private:
    bool canBeOpened() override;
    
    void paint(Graphics& g) override;
    void mouseDown(const MouseEvent& event) override;
    
    void openessChanged(bool newState) override;
    void itemDroppedIntoThisItem(ThumbnailBrowserItem* droppedItem) override;
    
    void valueTreeChildAdded(ValueTree& parent, ValueTree& childWhichHasBeenAdded) override;
    void valueTreeChildRemoved(ValueTree& parent, ValueTree& removedChild, int index) override;
    
    //fxToLookUp should be from the FXList of a category
    ValueTree lookUpAndFindFX(const ValueTree& fxToLookUp);
    
    ValueTree displayedTree;
    
    DataTreeManager& treeManager;
};

class FXBrowserItem  : public FXAndCategoryBrowserItem
{
public:
    FXBrowserItem(const ValueTree& treeToDisplay, DataTreeManager& dataManager);
    ~FXBrowserItem();
    
private:
    void resized() override;
    void paintOverlay(Graphics& g) override;
    void mouseMove(const MouseEvent& event) override;
    
    class PlayButton  : public Button
    {
    public:
        PlayButton();
        ~PlayButton();
        
    private:
        void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    };
    
    bool mouseOver = false;
    
    PlayButton playButton;
};

//==============================================================================

//Browser with some custom mouse controls
class FXAndCategoryBrowser  : public HierarchicalThumbnailBrowser,
                              public FileDragAndDropTarget,
                              public DragAndDropTarget
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
    void displayedItemChanged() override;
    void rootItemChanged() override;
    void paintSection(Graphics& g, int width, int height, const String& sectionName, int sectionIndex) override;
    
    bool isInterestedInDragSource(const SourceDetails& details) override;
    void itemDragMove(const SourceDetails& details) override;
    void itemDropped(const SourceDetails& details) override;
    
    void addCategoryToDisplayedTree();
    
    int sectionSelectionRule(ThumbnailBrowserItem* item) const;
    
    //Will return std::nullopt if on an item
    std::optional<Point<int>> getMiddleOfTwoItems(const Point<int>& referencePoint);
    
    Point<int> currentPossibleDragPoint;
    
    bool filesBeingDragged = false;
    
    class TitleBar : public Component
    {
    public:
        TitleBar(FXAndCategoryBrowser& titleOwner);
        ~TitleBar();
        
    private:
        void paint(Graphics& g) override;
        void resized() override;
        
        //Leave blank for displayed item - returns the end point of drawing
        int recursivelyPlaceItems(FXAndCategoryBrowserItem* itemToDraw = nullptr, int depth = 0);
        
        FXAndCategoryBrowser& owner;
        
        class TitleBarLabel : public Label,
                              public DragAndDropTarget
        {
        public:
            TitleBarLabel(TitleBar& titleBar);
            ~TitleBarLabel();
            
            void setItemToDisplay(FXAndCategoryBrowserItem* newItem);
            FXAndCategoryBrowserItem* getItemToDisplay() const;
            
        private:
            void mouseDoubleClick(const MouseEvent& event) override;
            bool isInterestedInDragSource(const SourceDetails& details) override;
            void itemDropped(const SourceDetails& details) override;
            
            FXAndCategoryBrowserItem* displayedItem = nullptr;
            TitleBar& titleBarOwner;
        };
        
        std::vector<std::unique_ptr<TitleBarLabel>> itemNameLabels;
    };
};
