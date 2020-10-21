#include "MainComponent.h"

FXAndCategoryBrowserItem::FXAndCategoryBrowserItem(const ValueTree& treeToDisplay, DataTreeManager& dataManager)  : displayedTree(treeToDisplay), treeManager(dataManager)
{
    displayedTree.addListener(this);
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
    if(newState)
    {
        clearSubItems();
        
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
                    lookUpAndAddFXAsChild(fxChild);
                }
            }
        }
    }
}

void FXAndCategoryBrowserItem::valueTreeChildAdded(ValueTree& parent, ValueTree& childWhichHasBeenAdded)
{
    //Sub-Cat added
    if(parent == displayedTree && childWhichHasBeenAdded.getType().toString() == "Category")
    {
        addNewSubItem(new FXAndCategoryBrowserItem(childWhichHasBeenAdded, treeManager));
    }
    
    //If it was an FX that was added
    else if(parent == displayedTree.getChildWithName("FXList"))
    {
        lookUpAndAddFXAsChild(childWhichHasBeenAdded);
    }
}

void FXAndCategoryBrowserItem::lookUpAndAddFXAsChild(const ValueTree& fxToLookUp)
{
    //This should only be passing in trees that are from a categories FX list
    assert(fxToLookUp.getParent().getType().toString() == "FXList");
    
    ValueTree fxChildToAdd = treeManager.getTreeFormatter(DataTreeManager::TreeType::FXTree)->getTreeWithID(fxToLookUp.getProperty("IDNum"));
    
    addNewSubItem(new FXAndCategoryBrowserItem(fxChildToAdd, treeManager));
}

//==============================================================================
FXAndCategoryBrowser::FXAndCategoryBrowser()
{
    
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
}

void FXAndCategoryBrowser::mouseDown(const MouseEvent& event)
{
    if(event.mods.isRightButtonDown())
    {
        PopupMenu menu;
    
        menu.addItem("New Sub-Category", std::bind(&FXAndCategoryBrowser::addCategoryToDisplayedTree, this));
        
        menu.show();
        update();
    }
}

bool FXAndCategoryBrowser::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void FXAndCategoryBrowser::fileDragMove(const StringArray& files, int x, int y)
{
    //Bounds not working
    if(x > getTitleBarHeight())
    {
        filesBeingDragged = true;
    }
    else
    {
        filesBeingDragged = false;
    }
    repaint();
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

void FXAndCategoryBrowser::addCategoryToDisplayedTree()
{
    FXAndCategoryBrowserItem* displayedItem = dynamic_cast<FXAndCategoryBrowserItem*>(getDisplayedItem());
    
    ValueTree treeToAddTo = displayedItem->getDisplayedTree();
    
    treeToAddTo.addChild(ValueTree("Category"), -1, nullptr);
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
