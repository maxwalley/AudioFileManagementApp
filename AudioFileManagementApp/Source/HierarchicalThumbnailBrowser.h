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

class ThumbnailBrowserItem  : public Component
{
public:
    ThumbnailBrowserItem();
    virtual ~ThumbnailBrowserItem();
    
    virtual bool canBeOpened()=0;
    
    void addNewSubItem(ThumbnailBrowserItem* newSubItem);
    void removeSubItem(ThumbnailBrowserItem* itemToRemove);
    void removeSubItem(int indexToRemove);
    void clearSubItems();
    
    //If index out of range will return nullptr
    ThumbnailBrowserItem* getItemAtIndex(int index) const;
    int getNumberOfSubItems() const;
    
private:
    virtual void paint(Graphics& g) override{};
    
    //True = open
    virtual void openessChanged(bool newState){};
    
    std::vector<std::unique_ptr<ThumbnailBrowserItem>> subItems;
    
    bool isOpen;
    
    friend class HierarchicalThumbnailBrowser;
};



class HierarchicalThumbnailBrowser  : public Component,
                                      public Button::Listener
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
    
    void setItemSize(const Size& newSize);
    Size getItemSize() const;
    
    void setHorizontalGapBetweenItems(int newGap);
    int getHorizontalGapBetweenItems() const;
    
    void setVerticalGapBetweenItems(int newGap);
    int getVerticalGapBetweenItems() const;
    
    void setTitleBarHeight(int newHeight);
    int getTitleBarHeight() const;
    
    void setTitleBarText(const String& newText);
    String getTitleBarText() const;
    
    void update();

protected:
    virtual void paintTitleBar(Graphics& g, int width, int height);
    
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
        
        void calculateAndResize();
        
    private:
        void paint (juce::Graphics& g) override;
        void resized() override;
        
        int calculateHowManyItemsPerRow() const;
        
        HierarchicalThumbnailBrowser& owner;
        int numItemsPerRow = 0;
    };
    
    std::unique_ptr<ThumbnailBrowserItem> currentDisplayedItem;
    
    Displayer contentDisplayer;
    Viewport viewport;
    
    Size itemSize;
    int horizontalGapBetweenItems = 10;
    int verticalGapBetweenItems = 15;
    int titleBarHeight = 22;
    String titleBarText = "";
    
    TextButton testButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HierarchicalThumbnailBrowser)
};
