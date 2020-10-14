/*
  ==============================================================================

    HierarchicalThumbnailBrowser.cpp
    Created: 14 Oct 2020 1:49:53pm
    Author:  Max Walley

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HierarchicalThumbnailBrowser.h"

ThumbnailBrowserItem::~ThumbnailBrowserItem()
{
    
}

void ThumbnailBrowserItem::addNewSubItem(ThumbnailBrowserItem* newSubItem)
{
    subItems.push_back(std::unique_ptr<ThumbnailBrowserItem>(newSubItem));
}

void ThumbnailBrowserItem::removeSubItem(ThumbnailBrowserItem* itemToRemove)
{
    std::remove(subItems.begin(), subItems.end(), itemToRemove);
}

void ThumbnailBrowserItem::removeSubItem(int indexToRemove)
{
    if(indexToRemove >= getNumberOfSubItems())
    {
        //ERROR
        return;
    }
    
    std::remove(subItems.begin(), subItems.end(), subItems[indexToRemove]);
}

ThumbnailBrowserItem* ThumbnailBrowserItem::getItemAtIndex(int index) const
{
    if(index < getNumberOfSubItems())
    {
        return subItems[index].get();
    }
    
    return nullptr;
}

int ThumbnailBrowserItem::getNumberOfSubItems() const
{
    return subItems.size();
}


//==============================================================================
HierarchicalThumbnailBrowser::HierarchicalThumbnailBrowser()
{
    
}

HierarchicalThumbnailBrowser::~HierarchicalThumbnailBrowser()
{
    
}

void HierarchicalThumbnailBrowser::setRootItem(ThumbnailBrowserItem* newRootItem)
{
    currentDisplayedItem = newRootItem;
    
    //Update display
}

void HierarchicalThumbnailBrowser::paint (juce::Graphics& g)
{
    g.fillAll(Colours::green);
}

void HierarchicalThumbnailBrowser::resized()
{

}
