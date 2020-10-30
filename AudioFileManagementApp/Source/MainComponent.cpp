#include "MainComponent.h"

//==============================================================================

MainComponent::MainComponent(const ValueTree& dataToDisplay, DataTreeManager& dataTreeManager)
{
    setSize (600, 400);
    
    addAndMakeVisible(browser);
    browser.addMouseListener(this, true);
    
    browser.setRootItem(std::make_unique<FXAndCategoryBrowserItem>(dataToDisplay.getChildWithName("Categories"), dataTreeManager));
    
    addAndMakeVisible(player);
    
    addAndMakeVisible(itemSizeSlider);
    itemSizeSlider.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    itemSizeSlider.setRange(50, 150);
    itemSizeSlider.addListener(this);
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
    browser.setBounds(0, 50, getWidth(), getHeight() - 100);
    player.setBounds(0, getHeight() - 50, getWidth(), 50);
    itemSizeSlider.setBounds(getWidth() / 3 * 2, 20, getWidth() / 3, 10);
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    int val = slider->getValue();
    
    browser.setItemSize({val, val / 5 * 6});
}
