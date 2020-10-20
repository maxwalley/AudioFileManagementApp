#include "MainComponent.h"

FXAndCategoryBrowserItem::FXAndCategoryBrowserItem(const ValueTree& treeToDisplay, DataTreeManager& dataManager)  : displayedTree(treeToDisplay), treeManager(dataManager)
{
    displayedTree.addListener(this);
}

FXAndCategoryBrowserItem::~FXAndCategoryBrowserItem()
{
    
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

void FXAndCategoryBrowser::mouseDown(const MouseEvent& event)
{
    DBG("TEST");
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
