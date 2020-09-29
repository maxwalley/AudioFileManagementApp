/*
  ==============================================================================

    AddFilesParameterEditor.h
    Created: 16 Sep 2020 10:53:00am
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "HierachicalListBrowser.h"

//==============================================================================
/*
*/
using namespace juce;

class ValueTreeItem  : public TreeViewItem,
                       public MouseListener,
                       public Label::Listener
{
public:
    ValueTreeItem(ValueTree treeToDisplay);
    ~ValueTreeItem();
    
    bool mightContainSubItems() override;
    
    String getUniqueName() const override;
    
    void itemOpennessChanged(bool isNowOpen) override;
    
    Component* createItemComponent() override;
    
    void mouseDown(const MouseEvent& event) override;
    
    void labelTextChanged(Label* label) override;
    
    ValueTree getShownTree() const;
    
private:
    ValueTree tree;
};


class AddFilesParameterEditor  : public Component,
                                 public Button::Listener
{
public:
    AddFilesParameterEditor(juce::ValueTree currentData);
    ~AddFilesParameterEditor() override;

    void paint (Graphics&) override;
    void resized() override;

private:
    
    void buttonClicked(Button* button) override;
    
    bool keyPressed(const KeyPress& key) override;
    
    void deleteSelectedItems();
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddFilesParameterEditor)
};
