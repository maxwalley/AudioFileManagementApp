/*
  ==============================================================================

    HierarchicalThumbnailBrowser.h
    Created: 14 Oct 2020 1:49:53pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
using namespace juce;

class HierarchicalThumbnailBrowser;

class ThumbnailBrowserItem  : public Component,
                              public DragAndDropTarget
{
public:
    ThumbnailBrowserItem();
    virtual ~ThumbnailBrowserItem();
    
    virtual bool canBeOpened()=0;
    
    void addNewSubItem(ThumbnailBrowserItem* newSubItem);
    void removeSubItem(ThumbnailBrowserItem* itemToRemove);
    void removeSubItem(int indexToRemove);
    
    ThumbnailBrowserItem* removeSubItemAndRelease(ThumbnailBrowserItem* itemToRemove);
    ThumbnailBrowserItem* removeSubItemAndRelease(int indexToRemove);
    void clearSubItems();
    
    //If index out of range will return nullptr
    ThumbnailBrowserItem* getItemAtIndex(int index) const;
    int getNumberOfSubItems() const;
    
    void setOwner(HierarchicalThumbnailBrowser* newOwner);
    HierarchicalThumbnailBrowser* getOwner() const;
    
    ThumbnailBrowserItem* getParent() const;
    
private:
    virtual void paint(Graphics& g) override{};
    
    //True = open
    virtual void openessChanged(bool newState){};
    
    virtual void itemDoubleClicked(const MouseEvent& event){};
    
    virtual void itemDroppedIntoThisItem(ThumbnailBrowserItem* itemThatHasBeenDropped){};
    
    void mouseDrag(const MouseEvent& event) override;
    void mouseDoubleClick(const MouseEvent& event) override;
    
    bool isInterestedInDragSource(const SourceDetails& details) override;
    virtual void itemDragEnter(const SourceDetails& details) override;
    virtual void itemDragExit(const SourceDetails& details) override;
    void itemDropped(const SourceDetails& details) override;
    
    int getIndexOfSubItem(ThumbnailBrowserItem* itemToGetIndexOf) const;
    
    std::vector<std::unique_ptr<ThumbnailBrowserItem>> subItems;
    
    bool isOpen;
    
    HierarchicalThumbnailBrowser* owner = nullptr;
    ThumbnailBrowserItem* parent = nullptr;
    
    int sectionID = 0;
    
    friend class HierarchicalThumbnailBrowser;
};



class HierarchicalThumbnailBrowser  : public Component,
                                      public Button::Listener,
                                      public DragAndDropContainer
{
public:
    
    struct Size
    {
        int width;
        int height;
    };
    
    enum ColourIds
    {
        backgroundColourId = 0,
        titleBarBackgroundColourId = 1,
        titleBarTextColourId = 2
    };
    
    HierarchicalThumbnailBrowser();
    ~HierarchicalThumbnailBrowser() override;
    
    //Passing nullptr will clear the browser
    void setRootItem(std::unique_ptr<ThumbnailBrowserItem> newRootItem);
    ThumbnailBrowserItem* getRootItem() const;
    
    void setDisplayedItem(ThumbnailBrowserItem* newItemToDisplay);
    ThumbnailBrowserItem* getDisplayedItem() const;
    
    void setItemSize(const Size& newSize);
    Size getItemSize() const;
    
    void setHorizontalGapBetweenItems(int newGap);
    int getHorizontalGapBetweenItems() const;
    
    void setVerticalGapBetweenItems(int newGap);
    int getVerticalGapBetweenItems() const;
    
    void setTitleBarHeight(int newHeight);
    int getTitleBarHeight() const;
    
    void update();
    
    //This will mean paintTitleBar does nothing if this is not nullptr
    void setTitleBarComponent(std::unique_ptr<Component> newTitleBar);
    Component* getTitleBarComponent() const;
    
    //-1 index means added to the end
    void addSection(const String& sectionName, int index = -1);
    void removeSection(const String& sectionName);
    void removeSection(int indexToRemove);
    int getIndexOfSection(const String& sectionName) const;
    const String getSectionNameAtIndex(int index) const;
    int getNumSections() const;
    
    void setGapBetweenSections(int newGap);
    int getGapBetweenSections() const;
    
    void setSectionSelectionRule(const std::function<int(ThumbnailBrowserItem*)>& newRule);
    
    int getNumSubItemsInSection(ThumbnailBrowserItem* itemToSearch, int sectionIndex) const;

protected:
    virtual void paintTitleBar(Graphics& g, int width, int height);
    virtual void paintSection(Graphics& g, int width, int height, const String& sectionName, int sectionIndex){};
    virtual void displayedItemChanged(){};
    virtual void rootItemChanged(){};
    virtual void colourChanged() override;
    
private:
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void buttonClicked(Button* button) override;
    
    class Displayer  : public Component
    {
    public:
        Displayer(HierarchicalThumbnailBrowser& ownerToDisplay);
        ~Displayer();
        
        void calculateAndResize(bool refreshChildList);
        
    private:
        void paint (juce::Graphics& g) override;
        void resized() override;
        
        int calculateHowManyItemsPerRow() const;
        void refreshChildrenComponents();
        
        HierarchicalThumbnailBrowser& owner;
        int numItemsPerRow = 0;
        
        struct sectionData
        {
            int sectionID;
            int numItems;
            int sectionHeight;
        };
        
        //Currently used section IDs and how many items are in that section
        std::vector<sectionData> sectionIds;
    };
    
    std::unique_ptr<ThumbnailBrowserItem> rootItem;
    
    ThumbnailBrowserItem* currentDisplayedItem = nullptr;
    
    Displayer contentDisplayer;
    Viewport viewport;
    
    Size itemSize;
    int horizontalGapBetweenItems = 10;
    int verticalGapBetweenItems = 15;
    int titleBarHeight = 22;
    
    TextButton testButton;
    
    std::unique_ptr<Component> titleBar = nullptr;
    
    StringArray sectionNames;
    
    int gapBetweenSections = 20;
    
    std::function<int(ThumbnailBrowserItem*)> sectionSelectionRule = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HierarchicalThumbnailBrowser)
};
