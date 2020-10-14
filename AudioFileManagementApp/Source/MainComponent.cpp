#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent(const ValueTree& dataToDisplay) : browser(dataToDisplay)
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
