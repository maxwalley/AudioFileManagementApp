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
class AddButton : public juce::Button
{
public:
    AddButton();
    ~AddButton();
    
private:
    void paintButton (juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
};


class HierachicalListBrowser  : public juce::Component,
                                public juce::Label::Listener,
                                public juce::Button::Listener
{
public:
    HierachicalListBrowser();
    ~HierachicalListBrowser() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void setDataToDisplay(juce::ValueTree newData);
    
    void refresh();
    
    void setFolderName(const juce::String& newName);
    
    juce::StringArray getAllHighlightedItems() const;
    
    void addTypesToIgnore(const juce::StringArray& listOfTypes);

protected:
    virtual void drawChildren(juce::Graphics& g, juce::ValueTree treeToDraw);
    
    virtual void drawLabels(juce::ValueTree treeToDraw);
    
    virtual int formatTree(juce::ValueTree inputTree, int startY = 0, int startIndentationIndex = 0);
    
    virtual void labelTextChanged(juce::Label* label) override;
    
private:
    void mouseDown(const juce::MouseEvent &event) override;
    
    void buttonClicked(juce::Button* button) override;
    
    void addChildrenToHighlights(juce::ValueTree tree);
    
    juce::ValueTree getBottomNode(juce::ValueTree inputTree);
    
    const int getNumberOfViewableNodes(juce::ValueTree inputTree) const;
    
    juce::ValueTree getNodeAtYVal(int yVal, juce::ValueTree treeToSearch) const;
    
    juce::StringArray getHighlightedForTree(juce::ValueTree treeToSearch) const;
    
    //Affect top tree will chose whether the top level of the tree is affected
    void setAllPropertiesInATree(juce::ValueTree inputTree, juce::Identifier property, juce::var val, bool affectTopTree, bool recursive);
    
    juce::ValueTree dataToDisplay;
    
    bool dataFormatted;
    
    std::vector<std::unique_ptr<juce::Label>> nameLabels;
    
    AddButton addButton;
    
    juce::String folderName;
    
    juce::StringArray typesToIgnore;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HierachicalListBrowser)
};
