/*
  ==============================================================================

    AddFilesParameterEditor.cpp
    Created: 16 Sep 2020 10:53:00am
    Author:  Max Walley

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AddFilesParameterEditor.h"

ValueTreeItem::ValueTreeItem(ValueTree treeToDisplay) : tree(treeToDisplay)
{
    
}

ValueTreeItem::~ValueTreeItem()
{
    
}

bool ValueTreeItem::mightContainSubItems()
{
    return tree.getNumChildren();
}

String ValueTreeItem::getUniqueName() const
{
    return tree.getProperty("Name").toString();
}

void ValueTreeItem::itemOpennessChanged(bool isNowOpen)
{
    if(isNowOpen)
    {
        if(!getNumSubItems())
        {
            std::for_each(tree.begin(), tree.end(), [this](ValueTree curTree)
            {
                addSubItem(new ValueTreeItem(curTree));
            });
        }
    }
}

Component* ValueTreeItem::createItemComponent()
{
    Label* newLabel = new Label();
    
    newLabel->setText(tree.getProperty("Name"), dontSendNotification);
    newLabel->setEditable(false, true);
    newLabel->addMouseListener(this, false);
    newLabel->addListener(this);
    
    return newLabel;
}

void ValueTreeItem::mouseDown(const MouseEvent& event)
{
    setSelected(!isSelected(), false, dontSendNotification);
    
    if(isSelected())
    {
        event.originalComponent->setColour(juce::Label::backgroundColourId, juce::Colours::black);
    }
    else
    {
        event.originalComponent->setColour(juce::Label::backgroundColourId, juce::Colours::silver);
    }
}

void ValueTreeItem::labelTextChanged(Label* label)
{
    tree.setProperty("Name", label->getText(), nullptr);
}

ValueTree ValueTreeItem::getShownTree() const
{
    return tree;
}

//==============================================================================
AddFilesParameterEditor::AddFilesParameterEditor(juce::ValueTree currentData) : dataToAddTo(currentData), newCatButton("New Catagory"), removeCatButton("Remove Catagory")
{
    addAndMakeVisible(titleLabel);
    titleLabel.setText("Parameters", dontSendNotification);
    titleLabel.setFont(Font(20));
    titleLabel.setJustificationType(Justification::centred);
    
    newVersionToggle.addListener(this);
    addAndMakeVisible(newVersionToggle);
    newVersionLabel.setText("Files are new versions of exisiting files", dontSendNotification);
    newVersionLabel.attachToComponent(&newVersionToggle, true);
    
    catagoryViewer.setRootItem(new ValueTreeItem(dataToAddTo.getChildWithName("Catagories")));
    catagoryViewer.setRootItemVisible(false);
    addAndMakeVisible(catagoryViewer);
    
    addAndMakeVisible(newCatButton);
    newCatButton.addListener(this);
    newCatButton.setTooltip("Add a new catagory to each of the highlighted items");
    
    addAndMakeVisible(removeCatButton);
    removeCatButton.addListener(this);
    //Remove catagory needs to remove it from the fx!!!
    removeCatButton.setTooltip("Remove all selected catagories. Warning: this will remove this catagory from all fx using it");
    
    addAndMakeVisible(nounLabel);
    nounLabel.setColour(Label::ColourIds::outlineColourId, Colours::black);
    nounLabel.setEditable(true);
    
    addAndMakeVisible(verbLabel);
    verbLabel.setColour(Label::ColourIds::outlineColourId, Colours::black);
    verbLabel.setEditable(true);
    
    addAndMakeVisible(descripEditor);
    descripEditor.setMultiLine(true);
    descripEditor.setColour(TextEditor::ColourIds::backgroundColourId, Colours::silver);
    descripEditor.setColour(TextEditor::outlineColourId, Colours::black);
    descripEditor.setTooltip("These words should describe the effect and be seperated with a ','");
}

AddFilesParameterEditor::~AddFilesParameterEditor()
{
    catagoryViewer.deleteRootItem();
}

void AddFilesParameterEditor::paint (juce::Graphics& g)
{
    if(!newVersionToggle.getToggleState())
    {
        g.drawText("Choose Catagories", 0, 80, getWidth(), 10, Justification::left);
        g.drawText("Noun", 10, 290, 120, 15, Justification::centred);
        g.drawText("Verb", getWidth() - 130, 290, 120, 15, Justification::centred);
        g.drawText("Descriptive Words", 10, 335, getWidth() - 20, 15, Justification::left);
    }
}

void AddFilesParameterEditor::resized()
{
    titleLabel.setBounds(getWidth() / 3, 10, getWidth() / 3, 40);
    
    newVersionToggle.setBounds(getWidth() / 6 * 5, 50, 22, 20);
    
    if(!newVersionToggle.getToggleState())
    {
        catagoryViewer.setBounds(0, 95, getWidth(), 155);
        newCatButton.setBounds(10, 260, 120, 20);
        removeCatButton.setBounds(getWidth() - 130, 260, 120, 20);
        nounLabel.setBounds(10, 305, 120, 20);
        verbLabel.setBounds(getWidth() - 130, 305, 120, 20);
        descripEditor.setBounds(10, 350, getWidth() - 20, 40);
    }
    else
    {
        catagoryViewer.setVisible(false);
    }
}

void AddFilesParameterEditor::buttonClicked(Button* button)
{
    if(button == &newVersionToggle)
    {
        resized();
    }
    
    else if(button == &newCatButton)
    {
        int numSelected = catagoryViewer.getNumSelectedItems();
        
        if(numSelected == 0)
        {
            ValueTreeItem* rootItem = dynamic_cast<ValueTreeItem*>(catagoryViewer.getRootItem());
            
            if(rootItem != nullptr)
            {
                ValueTree newChild("Catagory");
                newChild.setProperty("Name", "New Catagory", nullptr);
                rootItem->getShownTree().appendChild(newChild, nullptr);
                rootItem->addSubItem(new ValueTreeItem(newChild));
            }
            
            return;
        }
        
        for(int i = 0; i < numSelected; i++)
        {
            ValueTreeItem* selectedItem = dynamic_cast<ValueTreeItem*>(catagoryViewer.getSelectedItem(i));
            
            if(selectedItem == nullptr)
            {
                continue;
            }
            
            ValueTree newChild("Catagory");
            newChild.setProperty("Name", "New Catagory", nullptr);
            
            selectedItem->getShownTree().appendChild(newChild, nullptr);
            
            selectedItem->addSubItem(new ValueTreeItem(newChild));
        }
    }
    
    else if(button == &removeCatButton)
    {
        deleteSelectedItems();
    }
}

bool AddFilesParameterEditor::keyPressed(const KeyPress& key)
{
    if(key == KeyPress::deleteKey || key == KeyPress::backspaceKey)
    {
        deleteSelectedItems();
    }
    
    return true;
}

void AddFilesParameterEditor::deleteSelectedItems()
{
    int numSelected = catagoryViewer.getNumSelectedItems();
    
    if(numSelected == 0)
    {
        return;
    }
    
    if(AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, "Are you sure you want to delete this catagory?", "Deleting these catagories will delete them from any associated fx. Are you sure you want to continue?", "Continue", "Cancel", this))
    {
    /*
    for(int i = 0; i < numSelected; i++)
    {
        ValueTreeItem* selectedItem = dynamic_cast<ValueTreeItem*>(catagoryViewer.getSelectedItem(i));
        
        if(selectedItem == nullptr)
        {
            continue;
        }
        
        ValueTree selectedTree = selectedItem->getShownTree();
        
        selectedTree.getParent().removeChild(selectedTree, nullptr);
        
        selectedItem->getParentItem()->removeSubItem(selectedItem->getIndexInParent());
    }*/
        DBG("This is not yet implemented - catagories will need to be removed from fx");
    }
}
