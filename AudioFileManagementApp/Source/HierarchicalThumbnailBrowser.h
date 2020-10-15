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
    
    virtual void paintItem(Graphics& g, int width, int height){};
    
    void addNewSubItem(ThumbnailBrowserItem* newSubItem);
    void removeSubItem(ThumbnailBrowserItem* itemToRemove);
    void removeSubItem(int indexToRemove);
    
    //If index out of range will return nullptr
    ThumbnailBrowserItem* getItemAtIndex(int index) const;
    int getNumberOfSubItems() const;
    
private:
    std::vector<std::unique_ptr<ThumbnailBrowserItem>> subItems;
    
    bool isOpen;
};



class HierarchicalThumbnailBrowser  : public Component
{
public:
    HierarchicalThumbnailBrowser();
    ~HierarchicalThumbnailBrowser() override;
    
    //Passing nullptr will clear the browser
    void setRootItem(ThumbnailBrowserItem* newRootItem);
    
    ThumbnailBrowserItem* getRootItem() const;

private:
    
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    class Displayer  : public Component
    {
    public:
        Displayer(HierarchicalThumbnailBrowser& ownerToDisplay);
        ~Displayer();
        
        void calculateAndResize();
        
    private:
        void paint (juce::Graphics& g) override;
        void resized() override;
        
        HierarchicalThumbnailBrowser& owner;
    };
    
    ThumbnailBrowserItem* currentDisplayedItem;
    
    Displayer contentDisplayer;
    Viewport viewport;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HierarchicalThumbnailBrowser)
};
