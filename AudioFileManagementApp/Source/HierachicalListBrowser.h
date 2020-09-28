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

/*
 The class keeps a copy of the data to display and then the original one to edit. This means that loads of useless display propertys dont get swept up into the main ValueTree. Also this means multiple instances running of the same ValueTrees can run at once.
*/
class HierachicalListBrowser  : public juce::Component,
                                public juce::Label::Listener,
                                public juce::Button::Listener,
                                public juce::ValueTree::Listener
{
public:
    enum class ColourIds
    {
        textColourId = 1,
        symbolColourId = 2,
        backgroundColourId = 3,
        highlightColourId = 4,
        highlightTextColourId = 5
    };
    
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
    
    virtual void drawSymbol(juce::Graphics& g, int yStart, int indentation, bool hasChildren, bool isOpen);
    
private:
    void mouseDown(const juce::MouseEvent &event) override;
    
    void buttonClicked(juce::Button* button) override;
    
    void valueTreeChildAdded(juce::ValueTree& parentTree, juce::ValueTree& childWhichHasBeenAdded) override;
    
    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier& property) override;
    
    void addChildrenToHighlights(juce::ValueTree tree);
    
    juce::ValueTree getBottomNode(juce::ValueTree inputTree) const;
    
    const int getNumberOfViewableNodes(juce::ValueTree inputTree) const;
    
    juce::ValueTree getNodeAtYVal(int yVal, juce::ValueTree treeToSearch) const;
    
    juce::StringArray getHighlightedForTree(juce::ValueTree treeToSearch) const;
    
    //Comparison based on name property
    bool compareChildren(const juce::ValueTree& first, const juce::ValueTree& second) const;
    
    //Based on name
    juce::ValueTree findTreeInOtherTree(const juce::ValueTree& treeToFind, const juce::ValueTree& treeToSearch) const;
    
    //Affect top tree will chose whether the top level of the tree is affected
    void setAllPropertiesInATree(juce::ValueTree inputTree, const juce::Identifier& property, const juce::var& val, bool affectTopTree, bool recursive);
    
    juce::ValueTree originalData;
    juce::ValueTree dataToDisplay;
    
    bool dataFormatted;
    
    std::vector<std::unique_ptr<juce::Label>> nameLabels;
    
    AddButton addButton;
    
    juce::String folderName;
    
    juce::StringArray typesToIgnore;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HierachicalListBrowser)
};
