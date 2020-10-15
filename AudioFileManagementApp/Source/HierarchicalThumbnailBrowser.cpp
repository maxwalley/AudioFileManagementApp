/*
  ==============================================================================

    HierarchicalThumbnailBrowser.cpp
    Created: 14 Oct 2020 1:49:53pm
    Author:  Max Walley

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HierarchicalThumbnailBrowser.h"

ThumbnailBrowserItem::ThumbnailBrowserItem()  : isOpen(false)
{
    
}

ThumbnailBrowserItem::~ThumbnailBrowserItem()
{
    
}

void ThumbnailBrowserItem::addNewSubItem(ThumbnailBrowserItem* newSubItem)
{
    subItems.push_back(std::unique_ptr<ThumbnailBrowserItem>(newSubItem));
}

void ThumbnailBrowserItem::removeSubItem(ThumbnailBrowserItem* itemToRemove)
{
    std::for_each(subItems.begin(), subItems.end(), [this, &itemToRemove](const std::unique_ptr<ThumbnailBrowserItem>& item)
    {
        if(item.get() == itemToRemove)
        {
            std::remove(subItems.begin(), subItems.end(), item);
        }
    });
}

void ThumbnailBrowserItem::removeSubItem(int indexToRemove)
{
    if(indexToRemove >= getNumberOfSubItems())
    {
        //ERROR
        return;
    }
    
    subItems.erase(subItems.begin() + indexToRemove);
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
HierarchicalThumbnailBrowser::HierarchicalThumbnailBrowser()  : currentDisplayedItem(nullptr), contentDisplayer(*this)
{
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&contentDisplayer, false);
}

HierarchicalThumbnailBrowser::~HierarchicalThumbnailBrowser()
{
    
}

void HierarchicalThumbnailBrowser::setRootItem(ThumbnailBrowserItem* newRootItem)
{
    if(currentDisplayedItem == newRootItem)
    {
        return;
    }
    
    currentDisplayedItem = newRootItem;
    
    //Update display
    resized();
}

ThumbnailBrowserItem* HierarchicalThumbnailBrowser::getRootItem() const
{
    return currentDisplayedItem;
}

void HierarchicalThumbnailBrowser::paint (juce::Graphics& g)
{
    
}

void HierarchicalThumbnailBrowser::resized()
{
    viewport.setBounds(getLocalBounds());
    
    if(currentDisplayedItem == nullptr)
    {
        return;
    }
}

HierarchicalThumbnailBrowser::Displayer::Displayer(HierarchicalThumbnailBrowser& ownerToDisplay)  : owner(ownerToDisplay)
{
    calculateAndResize();
}

HierarchicalThumbnailBrowser::Displayer::~Displayer()
{
    calculateAndResize();
}

//Calculating and setting the size in this method rather than the resized() method means that the resized() method does not need to be called multiple times which means that each subcomponents resized will not be called over and over
void HierarchicalThumbnailBrowser::Displayer::calculateAndResize()
{
    //take the subitems from owner.getRootItem() aswell as owner.getItemSize() and work out size based on that. This is currently a placeholder.
    setSize(200, 1000);
}

void HierarchicalThumbnailBrowser::Displayer::paint(Graphics& g)
{
    g.fillAll(Colours::green);
}

void HierarchicalThumbnailBrowser::Displayer::resized()
{
    DBG("Resizing");
}
