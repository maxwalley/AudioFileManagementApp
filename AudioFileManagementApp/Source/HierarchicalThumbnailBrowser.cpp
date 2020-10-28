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
    std::unique_ptr<ThumbnailBrowserItem> newItem(newSubItem);
    newItem->parent = this;
    newItem->owner = owner;
    
    subItems.push_back(std::move(newItem));
}

void ThumbnailBrowserItem::removeSubItem(ThumbnailBrowserItem* itemToRemove)
{
    
    auto itemToDelete = std::remove_if(subItems.begin(), subItems.end(), [&itemToRemove](const std::unique_ptr<ThumbnailBrowserItem>& item)
    {
        return item.get() == itemToRemove;
    });
    
    subItems.erase(itemToDelete);
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

ThumbnailBrowserItem* ThumbnailBrowserItem::removeSubItemAndRelease(ThumbnailBrowserItem* itemToRemove)
{
    for (int i = 0; i < subItems.size(); i++)
    {
        if(itemToRemove == subItems[i].get())
        {
            ThumbnailBrowserItem* foundItem = subItems[i].release();
            subItems.erase(subItems.begin() + i);
            return foundItem;
        }
    }
    return nullptr;
}

ThumbnailBrowserItem* ThumbnailBrowserItem::removeSubItemAndRelease(int indexToRemove)
{
    if(indexToRemove >= subItems.size() || indexToRemove < 0)
    {
        return nullptr;
    }
    
    ThumbnailBrowserItem* itemToRemove = subItems[indexToRemove].release();
    subItems.erase(subItems.begin() + indexToRemove);
    return itemToRemove;
}

void ThumbnailBrowserItem::clearSubItems()
{
    subItems.clear();
}

void ThumbnailBrowserItem::moveSubItemFromItemIntoThis(ThumbnailBrowserItem* itemToMove)
{
    if(isAChildOf(itemToMove))
    {
        return;
    }
    
    addNewSubItem(itemToMove->getParent()->removeSubItemAndRelease(itemToMove));
}

bool ThumbnailBrowserItem::isAChildOf(ThumbnailBrowserItem* possibleParent) const
{
    ThumbnailBrowserItem* parentBeingChecked = getParent();
    
    while (parentBeingChecked != nullptr)
    {
        if(parentBeingChecked == possibleParent)
        {
            return true;
        }
        parentBeingChecked = parentBeingChecked->getParent();
    }
    
    return false;
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

void ThumbnailBrowserItem::setOwner(HierarchicalThumbnailBrowser* newOwner)
{
    owner = newOwner;
    
    //Iterator
    for(int i = 0; i < getNumberOfSubItems(); i++)
    {
        getItemAtIndex(i)->setOwner(owner);
    }
}

HierarchicalThumbnailBrowser* ThumbnailBrowserItem::getOwner() const
{
    return owner;
}

ThumbnailBrowserItem* ThumbnailBrowserItem::getParent() const
{
    return parent;
}

void ThumbnailBrowserItem::mouseDrag(const MouseEvent& event)
{
    DragAndDropContainer* dragContainer = DragAndDropContainer::findParentDragContainerFor(this);
    
    if(dragContainer != nullptr)
    {
        dragContainer->startDragging("ThumbnailItem", this);
    }
}

void ThumbnailBrowserItem::mouseDoubleClick(const MouseEvent& event)
{
    itemDoubleClicked(event);
    
    if(canBeOpened())
    {
        owner->setDisplayedItem(this);
    }
}

bool ThumbnailBrowserItem::isInterestedInDragSource(const SourceDetails& details)
{
    return details.description == "ThumbnailItem" && details.sourceComponent.get() != this;
}

void ThumbnailBrowserItem::itemDragEnter(const SourceDetails& details)
{
    
}

void ThumbnailBrowserItem::itemDragExit(const SourceDetails& details)
{
    
}

void ThumbnailBrowserItem::itemDropped(const SourceDetails& details)
{
    ThumbnailBrowserItem* draggedItem = dynamic_cast<ThumbnailBrowserItem*>(details.sourceComponent.get());
    
    if(draggedItem != nullptr)
    {
        itemDroppedIntoThisItem(draggedItem);
    }
}

int ThumbnailBrowserItem::getIndexOfSubItem(ThumbnailBrowserItem* itemToGetIndexOf) const
{
    for(int i = 0; i < getNumberOfSubItems(); i++)
    {
        if(subItems[i].get() == itemToGetIndexOf)
        {
            return i;
        }
    }
    
    return -1;
}

//==============================================================================
HierarchicalThumbnailBrowser::HierarchicalThumbnailBrowser()  : contentDisplayer(*this), itemSize{50, 50}, testButton("Test"), sectionNames("Undefined")
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

void HierarchicalThumbnailBrowser::setRootItem(std::unique_ptr<ThumbnailBrowserItem> newRootItem)
{
    if(rootItem == newRootItem)
    {
        return;
    }
    
    rootItem.reset(newRootItem.release());
    rootItem->setOwner(this);
    setDisplayedItem(rootItem.get());
    rootItemChanged();
    
    //Update display
    resized();
}

ThumbnailBrowserItem* HierarchicalThumbnailBrowser::getRootItem() const
{
    return rootItem.get();
}

void HierarchicalThumbnailBrowser::setDisplayedItem(ThumbnailBrowserItem* newItemToDisplay)
{
    if(newItemToDisplay != currentDisplayedItem)
    {
        if(currentDisplayedItem != nullptr)
        {
            currentDisplayedItem->openessChanged(false);
        }
        
        currentDisplayedItem = newItemToDisplay;
        
        if(currentDisplayedItem != nullptr)
        {
            currentDisplayedItem->openessChanged(true);
        }
        
        contentDisplayer.calculateAndResize(true);
        
        displayedItemChanged();
    }
}

ThumbnailBrowserItem* HierarchicalThumbnailBrowser::getDisplayedItem() const
{
    return currentDisplayedItem;
}

void HierarchicalThumbnailBrowser::setItemSize(const Size& newSize)
{
    itemSize = newSize;
    //Repaint content
    contentDisplayer.calculateAndResize(false);
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

void HierarchicalThumbnailBrowser::update()
{
    contentDisplayer.calculateAndResize(true);
}

void HierarchicalThumbnailBrowser::setTitleBarComponent(std::unique_ptr<Component> newTitleBar)
{
    titleBar.reset(newTitleBar.release());
    
    if(titleBar != nullptr)
    {
        addAndMakeVisible(titleBar.get());
    }
}

Component* HierarchicalThumbnailBrowser::getTitleBarComponent() const
{
    return titleBar.get();
}

void HierarchicalThumbnailBrowser::addSection(const String& sectionName, int index)
{
    if(index < 0 || index > sectionNames.size() - 1)
    {
        index = sectionNames.size() - 1;
    }
    
    sectionNames.insert(index, sectionName);
}

void HierarchicalThumbnailBrowser::removeSection(const String& sectionName)
{
    if(sectionName != "Undefined")
    {
        sectionNames.removeString(sectionName);
    }
}

void HierarchicalThumbnailBrowser::removeSection(int indexToRemove)
{
    if(indexToRemove < sectionNames.size() - 1)
    {
        sectionNames.remove(indexToRemove);
    }
}

int HierarchicalThumbnailBrowser::getIndexOfSection(const String& sectionName) const
{
    int indexFound = sectionNames.indexOf(sectionName);
    
    if(indexFound == sectionNames.size() - 1)
    {
        return -1;
    }
    
    return indexFound;
}

const String HierarchicalThumbnailBrowser::getSectionNameAtIndex(int index) const
{
    if(index >= sectionNames.size() - 1 || index < 0)
    {
        return String();
    }
    
    return sectionNames[index];
}

int HierarchicalThumbnailBrowser::getNumSections() const
{
    return sectionNames.size();
}

void HierarchicalThumbnailBrowser::setGapBetweenSections(int newGap)
{
    gapBetweenSections = newGap;
}

int HierarchicalThumbnailBrowser::getGapBetweenSections() const
{
    return gapBetweenSections;
}

void HierarchicalThumbnailBrowser::setSectionSelectionRule(const std::function<int(ThumbnailBrowserItem*)>& newRule)
{
    sectionSelectionRule = newRule;
}

int HierarchicalThumbnailBrowser::getNumSubItemsInSection(ThumbnailBrowserItem* itemToSearch, int sectionIndex) const
{
    int count = 0;
    
    for(int i = 0; i < itemToSearch->getNumberOfSubItems(); i++)
    {
        if(sectionSelectionRule == nullptr)
        {
            if(sectionIndex == 0)
            {
                count++;
            }
        }
        
        else
        {
            if(sectionSelectionRule(itemToSearch->getItemAtIndex(i)) == sectionIndex)
            {
                count++;
            }
        }
    }
    
    return count;
}

void HierarchicalThumbnailBrowser::paintTitleBar(Graphics& g, int width, int height)
{
    if(titleBar == nullptr)
    {
        g.setColour(findColour(titleBarBackgroundColourId));
        g.fillRect(0, 0, width, height);
    
        g.setColour(findColour(titleBarTextColourId));
    }
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
    
    viewport.setBounds(0, titleBarHeight, getWidth(), getHeight() - titleBarHeight);
    contentDisplayer.calculateAndResize(false);
    
    if(titleBar != nullptr)
    {
        titleBar->setBounds(0, 0, getWidth(), titleBarHeight);
    }
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

//Calculating and setting the size in this method rather than the resized() method means that the resized() method does not need to be called multiple times which means that recalculation will not occur multiple times
void HierarchicalThumbnailBrowser::Displayer::calculateAndResize(bool refreshItemList)
{
    if(refreshItemList)
    {
        refreshChildrenComponents();
    }
    
    numItemsPerRow = calculateHowManyItemsPerRow();
    
    //Iterate through the sections working out how big they are and adding the height on
    
    int componentHeight = 0;
    
    for (sectionData& id : sectionIds)
    {
        int numRowsInSection = ceil(float(id.numItems) / float(numItemsPerRow));
        id.sectionHeight = numRowsInSection * owner.getItemSize().height + (numRowsInSection - 1) * owner.getVerticalGapBetweenItems();
        componentHeight += id.sectionHeight;
    }
    
    if(componentHeight < owner.getHeight() - owner.getTitleBarHeight())
    {
        componentHeight = owner.getHeight() - owner.getTitleBarHeight();
    }
    
    int oldHeight = getHeight();
    
    setSize(owner.getWidth() - 8, componentHeight);
    
    //Forces a resize if one won't have occured because height is the same and components need to be redrawn
    if(oldHeight == getHeight() && refreshItemList)
    {
        resized();
    }
}

void HierarchicalThumbnailBrowser::Displayer::paint(Graphics& g)
{
    Point<int> currentOrigin(owner.getHorizontalGapBetweenItems(), owner.getVerticalGapBetweenItems());
    
    for (const sectionData& id : sectionIds)
    {
        g.setOrigin(currentOrigin);
        
        owner.paintSection(g, getWidth() - owner.getHorizontalGapBetweenItems(), id.sectionHeight, owner.getSectionNameAtIndex(id.sectionID), id.sectionID);
        
        currentOrigin.setY(id.sectionHeight + owner.getGapBetweenSections());
        currentOrigin.setX(0);
    }
}

void HierarchicalThumbnailBrowser::Displayer::resized()
{
    ThumbnailBrowserItem* displayedItem = owner.getDisplayedItem();
    
    int rowIndex = 0;
    
    Point<int> currentOrigin(owner.getHorizontalGapBetweenItems(), owner.getVerticalGapBetweenItems());
    
    for (const sectionData& id : sectionIds)
    {
        //Counts the number of items processed for this section
        int indexInThisSection = 0;
        
        for(int i = 0; i < displayedItem->getNumberOfSubItems(); i++)
        {
            ThumbnailBrowserItem* subItem = displayedItem->getItemAtIndex(i);
            
            if(subItem->sectionID != id.sectionID)
            {
                continue;
            }
            
            //Start of new row but not a new section
            if(indexInThisSection != 0 && indexInThisSection + 1 % numItemsPerRow == 1)
            {
                currentOrigin.setY(rowIndex * owner.getItemSize().height + owner.getVerticalGapBetweenItems());
                ++rowIndex;
                
                currentOrigin.setX(owner.getHorizontalGapBetweenItems());
            }
            else if(indexInThisSection != 0)
            {
                int newX = currentOrigin.getX() + owner.getItemSize().width + owner.getHorizontalGapBetweenItems();
                currentOrigin.setX(newX);
            }
            
            indexInThisSection++;
            
            subItem->setBounds(currentOrigin.getX(), currentOrigin.getY(), owner.itemSize.width, owner.itemSize.height);
        }
        
        //Moves origin on for next section
        currentOrigin.setXY(owner.getHorizontalGapBetweenItems(), currentOrigin.getY() + owner.getGapBetweenSections() + owner.getItemSize().height);
    }
}

int HierarchicalThumbnailBrowser::Displayer::calculateHowManyItemsPerRow() const
{
    return floor((owner.getWidth() - owner.horizontalGapBetweenItems - 8) / (owner.itemSize.width + owner.horizontalGapBetweenItems));
}

void HierarchicalThumbnailBrowser::Displayer::refreshChildrenComponents()
{
    removeAllChildren();
    
    ThumbnailBrowserItem* displayedItem = owner.getDisplayedItem();
    
    for(int i = 0; i < displayedItem->getNumberOfSubItems(); i++)
    {
        ThumbnailBrowserItem* subItem = displayedItem->getItemAtIndex(i);
        
        if(owner.sectionSelectionRule != nullptr)
        {
            int sectionID = owner.sectionSelectionRule(subItem);
            
            if(owner.getSectionNameAtIndex(sectionID).isEmpty())
            {
                //This is the undefined section ID
                sectionID = owner.getNumSections();
            }
            
            auto foundId = std::find_if(sectionIds.begin(), sectionIds.end(), [sectionID](sectionData& id)
            {
                if(id.sectionID == sectionID)
                {
                    return true;
                }
                return false;
            });
            
            if(foundId == sectionIds.end())
            {
                sectionIds.push_back({sectionID, 1, 0});
            }
            else
            {
                foundId->numItems++;
            }
            
            subItem->sectionID = sectionID;
        }
        
        addAndMakeVisible(displayedItem->getItemAtIndex(i));
    }
}
