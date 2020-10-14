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

class ThumbnailBrowserItem
{
public:
    virtual ~ThumbnailBrowserItem();
    
    virtual bool canBeOpened()=0;
    
    void addNewSubItem(ThumbnailBrowserItem* newSubItem);
    void removeSubItem(ThumbnailBrowserItem* itemToRemove);
    void removeSubItem(int indexToRemove);
    
    //If index out of range will return nullptr
    ThumbnailBrowserItem* getItemAtIndex(int index) const;
    int getNumberOfSubItems() const;
    
private:
    std::vector<std::unique_ptr<ThumbnailBrowserItem>> subItems;
};



class HierarchicalThumbnailBrowser  : public juce::Component
{
public:
    HierarchicalThumbnailBrowser();
    ~HierarchicalThumbnailBrowser() override;
    
    //Passing nullptr will clear the browser
    void setRootItem(ThumbnailBrowserItem* newRootItem);

private:
    
    void paint (juce::Graphics&) override;
    void resized() override;
    
    ThumbnailBrowserItem* currentDisplayedItem;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HierarchicalThumbnailBrowser)
};
