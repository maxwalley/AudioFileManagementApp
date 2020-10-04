/*
  ==============================================================================

    AddFilesParameterEditor.h
    Created: 16 Sep 2020 10:53:00am
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
using namespace juce;

class ValueTreeItem  : public TreeViewItem,
                       public MouseListener,
                       public Label::Listener,
                       public ValueTree::Listener
{
public:
    ValueTreeItem(ValueTree treeToDisplay);
    ~ValueTreeItem();
    
    bool mightContainSubItems() override;
    
    String getUniqueName() const override;
    
    Component* createItemComponent() override;
    
    void paintItem(Graphics& g, int width, int height) override;
    
    void itemSelectionChanged(bool isSelected) override;
    
    void mouseDown(const MouseEvent& event) override;
    
    void labelTextChanged(Label* label) override;
    
    void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    
    void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int index) override;
    
    void valueTreeChildOrderChanged(ValueTree& parentTree, int oldIndex, int newIndex) override;
    
    ValueTree getShownTree() const;
    
    void refreshChildren();
    
private:
    ValueTree tree;
};


class AddFilesParameterEditor  : public Component,
                                 public Button::Listener,
                                 public Label::Listener,
                                 public TextEditor::Listener
{
public:
    enum class KeywordType
    {
        noun,
        verb,
        other
    };
    
    AddFilesParameterEditor(juce::ValueTree currentData);
    ~AddFilesParameterEditor() override;

    void paint (Graphics&) override;
    void resized() override;
    
    void setDataToShow(ValueTree newData);
    
    class Listener
    {
    public:
        Listener(){};
        virtual ~Listener(){};
        
        virtual void dataChanged(KeywordType specificDataFieldChanged)=0;
    };
    
    void addListener(Listener* newListener);
    void removeListener(Listener* listenerToRemove);
    
    StringArray getTextFromComponent(KeywordType typeOfDataToFetch) const;

private:
    
    void mouseDown(const MouseEvent& event) override;
    
    void buttonClicked(Button* button) override;
    
    bool keyPressed(const KeyPress& key) override;
    
    void labelTextChanged(Label* label) override;
    
    void textEditorFocusLost(TextEditor& editor) override;
    
    void deleteSelectedItems();
    
    void lookAndHighlightCatagory(const String& catagory, TreeViewItem* treeToSearch);
    
    StringArray seperateTextByCommaIntoArray(const String& textToSeperate) const;
    
    void seperateTextByCommaIntoTreeChildren(const String& textToSeperate, ValueTree treeToAddTo, const Identifier& newChildrenTypes) const;
    
    //-1 if it doesnt find one
    int getIndexOfFirstKeywordType(KeywordType typeToLookFor) const;
    
    //-1 if it doesnt find one
    int getIndexOfLastKeywordType(KeywordType typeToLookFor) const;
    
    Label titleLabel;
    
    ToggleButton newVersionToggle;
    Label newVersionLabel;
    
    TreeView catagoryViewer;
    ValueTree dataToAddTo;
    
    TextButton newCatButton;
    TextButton removeCatButton;
    
    Label nounLabel;
    Label verbLabel;
    
    TextEditor descripEditor;
    
    ValueTree dataToShow;
    
    std::vector<Listener*> listeners;
    
    String lastDescripEditorText;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddFilesParameterEditor)
};
