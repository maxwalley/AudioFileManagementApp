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

class HierachicalListBrowser  : public juce::Component,
                                public juce::Label::Listener
{
public:
    HierachicalListBrowser();
    ~HierachicalListBrowser() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void setDataToDisplay(juce::ValueTree newData);
    
    void refresh();

protected:
    virtual void drawChildren(juce::Graphics& g, juce::ValueTree treeToDraw);
    
    virtual void drawLabels(juce::ValueTree treeToDraw);
    
    virtual int formatTree(juce::ValueTree inputTree, int startY = 0, int startIndentationIndex = 0);
    
    virtual void labelTextChanged(juce::Label* label) override;
    
private:
    void mouseDown(const juce::MouseEvent &event) override;
    
    juce::ValueTree getBottomNode(juce::ValueTree inputTree);
    
    const int getNumberOfViewableNodes(juce::ValueTree inputTree) const;
    
    juce::ValueTree getNodeAtYVal(int yVal, juce::ValueTree treeToSearch) const;
    
    juce::ValueTree dataToDisplay;
    
    bool dataFormatted;
    
    std::vector<std::unique_ptr<juce::Label>> nameLabels;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HierachicalListBrowser)
};
