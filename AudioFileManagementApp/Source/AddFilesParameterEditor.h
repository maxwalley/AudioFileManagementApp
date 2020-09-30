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
                       public Label::Listener,
                       public ValueTree::Listener
{
public:
    ValueTreeItem(ValueTree treeToDisplay);
    ~ValueTreeItem();
    
    bool mightContainSubItems() override;
    
    String getUniqueName() const override;
    
    Component* createItemComponent() override;
    
    void itemSelectionChanged(bool isSelected) override;
    
    void mouseDown(const MouseEvent& event) override;
    
    void labelTextChanged(Label* label) override;
    
    void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    
    void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int index) override;
    
    void valueTreeChildOrderChanged(ValueTree& parentTree, int oldIndex, int newIndex) override;
    
    ValueTree getShownTree() const;
    
    void refreshChildren();
    
    class Listener
    {
    public:
        virtual ~Listener(){};
        
        virtual void ItemSelected(ValueTreeItem* item, bool selection) = 0;
    };
    
    void addListener(Listener* newListener);
    void removeListener(Listener* listenerToRemove);
    
private:
    ValueTree tree;
    
    std::vector<Listener*> listeners;
};


class ItemLabel  : public Label,
                   public ValueTreeItem::Listener
{
public:
    ItemLabel();
    ~ItemLabel();
    
    void ItemSelected(ValueTreeItem* item, bool selection) override;
    
};


class AddFilesParameterEditor  : public Component,
                                 public Button::Listener
{
public:
    AddFilesParameterEditor(juce::ValueTree currentData);
    ~AddFilesParameterEditor() override;

    void paint (Graphics&) override;
    void resized() override;
    
    void setDataToShow(ValueTree newData);

private:
    
    void buttonClicked(Button* button) override;
    
    bool keyPressed(const KeyPress& key) override;
    
    void deleteSelectedItems();
    
    void lookAndHighlightCatagory(const String& catagory, TreeViewItem* treeToSearch);
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddFilesParameterEditor)
};
