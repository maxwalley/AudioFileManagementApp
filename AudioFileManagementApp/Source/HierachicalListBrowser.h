/*
  ==============================================================================

    HierachicalListBrowser.h
    Created: 16 Sep 2020 11:41:39am
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/

class HierachicalListBrowser  : public juce::Component
{
public:
    HierachicalListBrowser();
    ~HierachicalListBrowser() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void setDataToDisplay(juce::ValueTree newData);

private:
    int drawChildren(juce::Graphics& g, juce::ValueTree treeToDraw, int startY = 0, int indentationIndex = 0);
    
    void mouseDown(const juce::MouseEvent &event) override;
    
    juce::ValueTree getBottomNode(juce::ValueTree inputTree);
    
    juce::ValueTree dataToDisplay;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HierachicalListBrowser)
};
