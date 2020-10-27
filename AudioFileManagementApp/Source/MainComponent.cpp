#include "MainComponent.h"

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
    
    if(displayedTree.getType().toString() == "Category")
    {
        g.drawText(displayedTree.getProperty("Name"), 0, getHeight() - 10, getWidth(), 10, Justification::centred);
    }
    else if (displayedTree.getType().toString() == "FX")
    {
        g.drawText(displayedTree.getProperty("Path"), 0, getHeight() - 10, getWidth(), 10, Justification::centred);
    }
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
    
    addNewSubItem(dropped->getParent()->removeSubItemAndRelease(dropped));
    
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
    if(event.mods.isRightButtonDown())
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
    getTitleBarComponent()->repaint();
}

void FXAndCategoryBrowser::rootItemChanged()
{
    getTitleBarComponent()->repaint();
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
    String textToDisplay;
        
    if(owner.getDisplayedItem() == owner.getRootItem())
    {
        textToDisplay = "Root";
    }
    else
    {
        FXAndCategoryBrowserItem* displayedItem = dynamic_cast<FXAndCategoryBrowserItem*>(owner.getDisplayedItem());
        
        textToDisplay = displayedItem->getDisplayedTree().getProperty("Name");
    }
        
    g.drawText(textToDisplay, 0, 0, getWidth(), getHeight(), Justification::left);
}

//==============================================================================

MainComponent::MainComponent(const ValueTree& dataToDisplay, DataTreeManager& dataTreeManager)
{
    setSize (600, 400);
    
    addAndMakeVisible(browser);
    browser.addMouseListener(this, true);
    
    browser.setRootItem(std::make_unique<FXAndCategoryBrowserItem>(dataToDisplay.getChildWithName("Categories"), dataTreeManager));
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    
}

void MainComponent::resized()
{
    browser.setBounds(0, 40, getWidth(), getHeight() - 80);
}
