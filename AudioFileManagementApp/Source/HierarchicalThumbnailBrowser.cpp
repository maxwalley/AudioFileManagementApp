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
HierarchicalThumbnailBrowser::HierarchicalThumbnailBrowser()  : currentDisplayedItem(nullptr), contentDisplayer(*this), itemSize{50, 50}, testButton("Test")
{
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&contentDisplayer, false);
    
    //Default colours
    setColour(backgroundColourId, Colours::silver);
    setColour(titleBarBackgroundColourId, Colours::darkgrey);
    setColour(titleBarTextColourId, Colours::black);
    
    addAndMakeVisible(testButton);
    testButton.addListener(this);
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

void HierarchicalThumbnailBrowser::setItemSize(const Size& newSize)
{
    itemSize = newSize;
    //Repaint content
}

HierarchicalThumbnailBrowser::Size HierarchicalThumbnailBrowser::getItemSize() const
{
    return itemSize;
}

void HierarchicalThumbnailBrowser::setHorizontalGapBetweenItems(int newGap)
{
    horizontalGapBetweenItems = newGap;
}

int HierarchicalThumbnailBrowser::getHorizontalGapBetweenItems() const
{
    return horizontalGapBetweenItems;
}

void HierarchicalThumbnailBrowser::setVerticalGapBetweenItems(int newGap)
{
    verticalGapBetweenItems = newGap;
}

int HierarchicalThumbnailBrowser::getVerticalGapBetweenItems() const
{
    return verticalGapBetweenItems;
}

void HierarchicalThumbnailBrowser::setTitleBarHeight(int newHeight)
{
    titleBarHeight = newHeight;
}

int HierarchicalThumbnailBrowser::getTitleBarHeight() const
{
    return titleBarHeight;
}

void HierarchicalThumbnailBrowser::setTitleBarText(const String& newText)
{
    if(titleBarText != newText)
    {
        titleBarText = newText;
        repaint(0, 0, getWidth(), titleBarHeight);
    }
}

String HierarchicalThumbnailBrowser::getTitleBarText() const
{
    return titleBarText;
}

void HierarchicalThumbnailBrowser::paintTitleBar(Graphics& g, int width, int height)
{
    g.setColour(findColour(titleBarBackgroundColourId));
    g.fillRect(0, 0, width, height);
    
    g.setColour(findColour(titleBarTextColourId));
    g.drawText(titleBarText, 5, 0, width, height, Justification::left);
}

void HierarchicalThumbnailBrowser::colourChanged()
{
    repaint();
}

void HierarchicalThumbnailBrowser::paint (juce::Graphics& g)
{
    g.fillAll(findColour(backgroundColourId));
    
    paintTitleBar(g, getWidth(), titleBarHeight);
}

void HierarchicalThumbnailBrowser::resized()
{
    testButton.setBounds(0, getHeight() - 20, 100, 20);
    
    if(currentDisplayedItem == nullptr)
    {
        return;
    }
    
    viewport.setBounds(getLocalBounds());
    contentDisplayer.calculateAndResize();
}

void HierarchicalThumbnailBrowser::buttonClicked(Button* button)
{
    
}

HierarchicalThumbnailBrowser::Displayer::Displayer(HierarchicalThumbnailBrowser& ownerToDisplay)  : owner(ownerToDisplay)
{
    
}

HierarchicalThumbnailBrowser::Displayer::~Displayer()
{
    
}

//Calculating and setting the size in this method rather than the resized() method means that the resized() method does not need to be called multiple times which means that each subcomponents resized will not be called over and over
void HierarchicalThumbnailBrowser::Displayer::calculateAndResize()
{
    int numRows = ceil(float(owner.getRootItem()->getNumberOfSubItems()) / float(calculateHowManyItemsPerRow()));
    
    int componentHeight = (numRows + 1) * (owner.getVerticalGapBetweenItems() + owner.getItemSize().height) + owner.getVerticalGapBetweenItems();
    
    setSize(owner.getWidth() - 8, componentHeight);
}

void HierarchicalThumbnailBrowser::Displayer::paint(Graphics& g)
{
    g.fillAll(Colours::green);
}

void HierarchicalThumbnailBrowser::Displayer::resized()
{
    DBG("Resizing");
}

int HierarchicalThumbnailBrowser::Displayer::calculateHowManyItemsPerRow() const
{
    return floor((getWidth() - owner.horizontalGapBetweenItems - 8) / (owner.itemSize.width + owner.horizontalGapBetweenItems));
}
