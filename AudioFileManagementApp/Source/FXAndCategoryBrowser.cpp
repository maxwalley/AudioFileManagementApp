/*
  ==============================================================================

    FXAndCategoryBrowser.cpp
    Created: 30 Oct 2020 2:53:16pm
    Author:  Max Walley

  ==============================================================================
*/

#include "FXAndCategoryBrowser.h"

FXAndCategoryBrowserItem::FXAndCategoryBrowserItem(const ValueTree& treeToDisplay, DataTreeManager& dataManager)  : displayedTree(treeToDisplay), treeManager(dataManager)
{
    displayedTree.addListener(this);
    
    for(const ValueTree& child : displayedTree)
    {
        if(child.getType().toString() == "Category")
        {
            addNewSubItem(new FXAndCategoryBrowserItem(child, treeManager));
        }
        
        else if(child.getType().toString() == "FXList")
        {
            for(const ValueTree& fxChild : child)
            {
                addNewSubItem(new FXAndCategoryBrowserItem(lookUpAndFindFX(fxChild), treeManager));
            }
        }
    }
}

FXAndCategoryBrowserItem::~FXAndCategoryBrowserItem()
{
    
}

ValueTree FXAndCategoryBrowserItem::getDisplayedTree() const
{
    return displayedTree;
}

bool FXAndCategoryBrowserItem::canBeOpened()
{
    if(displayedTree.getType().toString() == "FX")
    {
        return false;
    }
    
    //Category
    if(displayedTree.getNumChildren() > 1 || displayedTree.getChildWithName("FXList").getNumChildren() > 0)
    {
        return true;
    }
    
    return false;
}

void FXAndCategoryBrowserItem::paint(Graphics& g)
{
    g.setColour(Colours::black);
    g.drawRect(0, 0, getWidth(), getHeight());

    float heightToDraw = float((getHeight()) / 6.0) * 5.0;
    
    g.drawLine(0, heightToDraw, getWidth(), heightToDraw);
    
    g.drawText(displayedTree.getProperty("Name"), 0, heightToDraw, getWidth(), float(getHeight()) / 6.0, Justification::centred | Justification::verticallyCentred);
}

void FXAndCategoryBrowserItem::mouseDown(const MouseEvent& event)
{
    //Play
}

void FXAndCategoryBrowserItem::openessChanged(bool newState)
{
    
}

void FXAndCategoryBrowserItem::itemDroppedIntoThisItem(ThumbnailBrowserItem* droppedItem)
{
    FXAndCategoryBrowserItem* dropped = dynamic_cast<FXAndCategoryBrowserItem*>(droppedItem);
    ValueTree droppedTree = dropped->getDisplayedTree();
    
    //Makes sure that the dropped tree is not a parent of the displayed tree
    if(displayedTree.isAChildOf(droppedTree))
    {
        return;
    }
    
    moveSubItemFromItemIntoThis(droppedItem);
    
    droppedTree.getParent().removeChild(droppedTree, nullptr);
    displayedTree.addChild(droppedTree, -1, nullptr);
    
    getOwner()->update();
}

void FXAndCategoryBrowserItem::valueTreeChildAdded(ValueTree& parent, ValueTree& childWhichHasBeenAdded)
{
    ValueTree treeToAdd;
    
    bool add = false;
    
    //Sub-Cat added
    if(parent == displayedTree && childWhichHasBeenAdded.getType().toString() == "Category")
    {
        treeToAdd = childWhichHasBeenAdded;
        add = true;
    }
    
    //If it was an FX that was added
    else if(parent == displayedTree.getChildWithName("FXList"))
    {
        treeToAdd = lookUpAndFindFX(childWhichHasBeenAdded);
        add = true;
    }
    
    //Checks if this already exists as a sub-Item
    for(int i = 0; i < getNumberOfSubItems(); i++)
    {
        if(dynamic_cast<FXAndCategoryBrowserItem*>(getItemAtIndex(i))->getDisplayedTree() == treeToAdd)
        {
            add = false;
        }
    }
    
    if(add)
    {
        addNewSubItem(new FXAndCategoryBrowserItem(treeToAdd, treeManager));
    }
}

void FXAndCategoryBrowserItem::valueTreeChildRemoved(ValueTree& parent, ValueTree& removedChild, int index)
{
    for(int i = 0; i < getNumberOfSubItems(); i++)
    {
        FXAndCategoryBrowserItem* subItem = dynamic_cast<FXAndCategoryBrowserItem*>(getItemAtIndex(i));
        
        if(removedChild == subItem->getDisplayedTree())
        {
            removeSubItem(subItem);
            return;
        }
    }
}

ValueTree FXAndCategoryBrowserItem::lookUpAndFindFX(const ValueTree& fxToLookUp)
{
    //This should only be passing in trees that are from a categories FX list
    assert(fxToLookUp.getParent().getType().toString() == "FXList");
    
    return treeManager.getTreeFormatter(DataTreeManager::TreeType::FXTree)->getTreeWithID(fxToLookUp.getProperty("IDNum"));
}

//==============================================================================

FXAndCategoryBrowser::FXAndCategoryBrowser()  : currentPossibleDragPoint(0, 0)
{
    setTitleBarComponent(std::make_unique<TitleBar>(*this));
    setSectionSelectionRule(std::bind(&FXAndCategoryBrowser::sectionSelectionRule, this, std::placeholders::_1));
    
    addSection("Categories");
    addSection("FX");
    
    setSectionPadding(5, 5, 20, 5);
    
    setItemSize({75, 90});
}

FXAndCategoryBrowser::~FXAndCategoryBrowser()
{
    
}

void FXAndCategoryBrowser::paintOverChildren(Graphics& g)
{
    if(filesBeingDragged)
    {
        g.setColour(Colour::fromFloatRGBA(192.0, 192.0, 192.0, 0.7));
        g.fillRect(0, getTitleBarHeight(), getWidth(), getHeight() - getTitleBarHeight());
        g.setColour(Colours::black);
        g.setFont(Font(18));
        g.drawText("Drop Files To Add", 0, (getHeight() - getTitleBarHeight()) / 2 - 9, getWidth(), 18, Justification::centred | Justification::verticallyCentred);
    }
    
    if(currentPossibleDragPoint.getX() != 0 && currentPossibleDragPoint.getY() != 0)
    {
        g.setColour(Colours::black);
        g.drawLine(currentPossibleDragPoint.getX(), currentPossibleDragPoint.getY() + getTitleBarHeight() - getItemSize().height / 4, currentPossibleDragPoint.getX(), currentPossibleDragPoint.getY() + getTitleBarHeight() + getItemSize().height / 4, 1.5);
    }
}

void FXAndCategoryBrowser::mouseDown(const MouseEvent& event)
{
    if(event.mods.isRightButtonDown() && dynamic_cast<ThumbnailBrowserItem*>(event.originalComponent) == nullptr)
    {
        PopupMenu menu;
    
        menu.addItem("New Sub-Category", std::bind(&FXAndCategoryBrowser::addCategoryToDisplayedTree, this));
        
        menu.show();
    }
}

bool FXAndCategoryBrowser::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void FXAndCategoryBrowser::fileDragMove(const StringArray& files, int x, int y)
{
    bool oldFilesBeingDragged = filesBeingDragged;
    
    if(x > getTitleBarHeight())
    {
        filesBeingDragged = true;
    }
    else
    {
        filesBeingDragged = false;
    }
    
    if(filesBeingDragged != oldFilesBeingDragged)
    {
        repaint();
    }
}

void FXAndCategoryBrowser::fileDragExit(const StringArray& files)
{
    filesBeingDragged = false;
    repaint();
}

void FXAndCategoryBrowser::filesDropped(const StringArray &files, int x, int y)
{
    filesBeingDragged = false;
}

void FXAndCategoryBrowser::displayedItemChanged()
{
    getTitleBarComponent()->resized();
}

void FXAndCategoryBrowser::rootItemChanged()
{
    getTitleBarComponent()->resized();
}

void FXAndCategoryBrowser::paintSection(Graphics& g, int width, int height, const String& sectionName, int sectionIndex)
{
    g.drawRect(0, 0, width, height);
    
    g.drawText(sectionName, 5, 0, width, 20, Justification::left | Justification::verticallyCentred);
}

bool FXAndCategoryBrowser::isInterestedInDragSource(const SourceDetails& details)
{
    return details.description == "ThumbnailItem";
}

void FXAndCategoryBrowser::itemDragMove(const SourceDetails& details)
{
    Point<int> tempPoint = details.localPosition;
    
    tempPoint.setY(tempPoint.getY() - 20);
    
    std::optional<Point<int>> pointBetweenItems = getMiddleOfTwoItems(tempPoint);
    
    Point<int> oldPoint = currentPossibleDragPoint;
    
    if(pointBetweenItems == std::nullopt)
    {
        currentPossibleDragPoint.setXY(0, 0);
    }
    else
    {
        currentPossibleDragPoint = *pointBetweenItems;
    }
    
    if(oldPoint != currentPossibleDragPoint)
    {
        repaint();
    }
}

void FXAndCategoryBrowser::itemDropped(const SourceDetails& details)
{
    if(currentPossibleDragPoint.getX() != 0 && currentPossibleDragPoint.getY() != 0)
    {
        //Rearrange
        currentPossibleDragPoint.setXY(0, 0);
        repaint();
    }
}

void FXAndCategoryBrowser::addCategoryToDisplayedTree()
{
    
    
    FXAndCategoryBrowserItem* displayedItem = dynamic_cast<FXAndCategoryBrowserItem*>(getDisplayedItem());
    
    ValueTree treeToAddTo = displayedItem->getDisplayedTree();
    
    treeToAddTo.addChild(ValueTree("Category"), -1, nullptr);
    
    update();
}

int FXAndCategoryBrowser::sectionSelectionRule(ThumbnailBrowserItem* item) const
{
    FXAndCategoryBrowserItem* castedItem = dynamic_cast<FXAndCategoryBrowserItem*>(item);
    
    if(castedItem->getDisplayedTree().getType().toString() == "Category")
    {
        return 0;
    }
    else if(castedItem->getDisplayedTree().getType().toString() == "FX")
    {
        return 1;
    }
    
    return -1;
}

std::optional<Point<int>> FXAndCategoryBrowser::getMiddleOfTwoItems(const Point<int>& referencePoint)
{
    for(int i = 0; i < getDisplayedItem()->getNumberOfSubItems(); i++)
    {
        ThumbnailBrowserItem* item = getDisplayedItem()->getItemAtIndex(i);
        
        if(referencePoint.getY() >= item->getY() && referencePoint.getY() <= item->getY() + getItemSize().height)
        {
            //Ref point is in an item
            if(referencePoint.getX() >= item->getX() && referencePoint.getX() <= item->getX() + getItemSize().width)
            {
                return std::nullopt;
            }
            
            else if(referencePoint.getX() >= item->getX() - getHorizontalGapBetweenItems() && referencePoint.getX() <= item->getX())
            {
                Point<int> midPoint;
                
                midPoint.setY(item->getY() + getItemSize().height / 2);
                midPoint.setX(getHorizontalGapBetweenItems() / 2 + (item->getX() - getHorizontalGapBetweenItems()));
                
                return midPoint;
            }
        }
    }
    
    return std::nullopt;
}

FXAndCategoryBrowser::TitleBar::TitleBar(FXAndCategoryBrowser& newOwner)  : owner(newOwner)
{
    
}

FXAndCategoryBrowser::TitleBar::~TitleBar()
{
    
}

void FXAndCategoryBrowser::TitleBar::paint(Graphics& g)
{
    g.drawLine(0, getHeight(), getWidth(), getHeight());
}

void FXAndCategoryBrowser::TitleBar::resized()
{
    recursivelyPlaceItems();
}

int FXAndCategoryBrowser::TitleBar::recursivelyPlaceItems(FXAndCategoryBrowserItem* itemToDraw, int depth)
{
    if(itemToDraw == nullptr)
    {
        itemToDraw = dynamic_cast<FXAndCategoryBrowserItem*>(owner.getDisplayedItem());
    }
    
    int startX = 0;
    String textToDraw = "Root";
    
    //not displayed item
    if(depth != 0)
    {
        textToDraw += "->";
    }
    
    if(itemToDraw != owner.getRootItem())
    {
        startX = recursivelyPlaceItems(dynamic_cast<FXAndCategoryBrowserItem*>(itemToDraw->getParent()), depth + 1);
        textToDraw = itemToDraw->getDisplayedTree().getProperty("Name");
    }
    //Only applies to root - since root will have the largest depth
    else
    {
        int oldSize = itemNameLabels.size();
        
        if(oldSize <= depth + 1)
        {
            if(oldSize != depth + 1)
            {
                itemNameLabels.resize(depth + 1);
            }
            
            for (int i = 0; i < itemNameLabels.size(); i++)
            {
                std::unique_ptr<TitleBarLabel>& label = itemNameLabels[i];
                
                if(i >= oldSize)
                {
                    label = std::make_unique<TitleBarLabel>(*this);
                    addChildComponent(label.get());
                }
                
                label->setVisible(true);
            }
        }
        
        //Old size more than depth
        else
        {
            //Makes unused labels invisible
            std::for_each(itemNameLabels.begin() + depth + 1, itemNameLabels.end(), [](std::unique_ptr<TitleBarLabel>& label)
            {
                label->setVisible(false);
            });
        }
    }
    
    itemNameLabels[depth]->setItemToDisplay(itemToDraw);
    int textWidth = itemNameLabels[depth]->getFont().getStringWidth(textToDraw) + 5;
    itemNameLabels[depth]->setText(textToDraw, dontSendNotification);
    itemNameLabels[depth]->setBounds(startX, 0, textWidth, getHeight());
    
    return startX + textWidth;
}

FXAndCategoryBrowser::TitleBar::TitleBarLabel::TitleBarLabel(TitleBar& titleBar) : titleBarOwner(titleBar)
{
    
}

FXAndCategoryBrowser::TitleBar::TitleBarLabel::~TitleBarLabel()
{
    
}

void FXAndCategoryBrowser::TitleBar::TitleBarLabel::setItemToDisplay(FXAndCategoryBrowserItem* newItem)
{
    displayedItem = newItem;
}

FXAndCategoryBrowserItem* FXAndCategoryBrowser::TitleBar::TitleBarLabel::getItemToDisplay() const
{
    return displayedItem;
}

void FXAndCategoryBrowser::TitleBar::TitleBarLabel::mouseDoubleClick(const MouseEvent& event)
{
    titleBarOwner.owner.setDisplayedItem(displayedItem);
}

bool FXAndCategoryBrowser::TitleBar::TitleBarLabel::isInterestedInDragSource(const SourceDetails& details)
{
    return details.description == "ThumbnailItem";
}

void FXAndCategoryBrowser::TitleBar::TitleBarLabel::itemDropped(const SourceDetails& details)
{
    FXAndCategoryBrowserItem* droppedItem = dynamic_cast<FXAndCategoryBrowserItem*>(details.sourceComponent.get());
    
    if(droppedItem == nullptr || droppedItem == displayedItem || displayedItem->isAChildOf(droppedItem))
    {
        return;
    }
    
    ValueTree droppedTree = droppedItem->getDisplayedTree();
    
    displayedItem->moveSubItemFromItemIntoThis(droppedItem);
    
    droppedTree.getParent().removeChild(droppedTree, nullptr);
    displayedItem->getDisplayedTree().addChild(droppedTree, -1, nullptr);
}
