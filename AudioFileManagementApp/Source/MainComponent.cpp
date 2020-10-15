#include "MainComponent.h"

FXAndCategoryBrowserItem::FXAndCategoryBrowserItem(const ValueTree& treeToDisplay)  : displayedTree(treeToDisplay)
{
    
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

void FXAndCategoryBrowserItem::paintItem(Graphics& g, int width, int height)
{
    g.drawRect(0, 0, getWidth(), getHeight());
}

//==============================================================================
MainComponent::MainComponent(const ValueTree& dataToDisplay) : browser()
{
    setSize (600, 400);
    
    addAndMakeVisible(browser);
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
