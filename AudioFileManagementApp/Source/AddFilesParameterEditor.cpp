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
    
    return newLabel;
}

//==============================================================================
AddFilesParameterEditor::AddFilesParameterEditor(juce::ValueTree currentData) : dataToAddTo(currentData)
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
    }
}

void AddFilesParameterEditor::resized()
{
    titleLabel.setBounds(getWidth() / 3, 10, getWidth() / 3, 40);
    
    newVersionToggle.setBounds(getWidth() / 6 * 5, 50, 22, 20);
    
    if(!newVersionToggle.getToggleState())
    {
        catagoryViewer.setBounds(0, 95, getWidth(), getHeight() - 95);
    }
}

void AddFilesParameterEditor::buttonClicked(Button* button)
{
    if(button == &newVersionToggle)
    {
        repaint();
    }
}
