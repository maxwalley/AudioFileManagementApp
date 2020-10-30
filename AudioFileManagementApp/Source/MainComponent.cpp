#include "MainComponent.h"

//==============================================================================

MainComponent::MainComponent(const ValueTree& dataToDisplay, DataTreeManager& dataTreeManager)
{
    setSize (600, 400);
    
    addAndMakeVisible(browser);
    browser.addMouseListener(this, true);
    
    browser.setRootItem(std::make_unique<FXAndCategoryBrowserItem>(dataToDisplay.getChildWithName("Categories"), dataTreeManager));
    
    addAndMakeVisible(player);
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
    player.setBounds(0, getHeight() - 40, getWidth(), 40);
}
