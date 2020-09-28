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

var ValueTreeItem::getDragSourceDescription()
{
    return "drag";
}

bool ValueTreeItem::isInterestedInDragSource(const DragAndDropTarget::SourceDetails& dragSourceDetails)
{
    return dragSourceDetails.description == "drag";
}

void ValueTreeItem::itemDropped(const DragAndDropTarget::SourceDetails& dragSourceDetails, int insertIndex)
{
    
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
AddFilesParameterEditor::AddFilesParameterEditor(juce::ValueTree currentData) : dataToAddTo(currentData), newCatButton("New Cat")
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
        catagoryViewer.setBounds(0, 95, getWidth(), getHeight() - 150);
        
        newCatButton.setBounds(0, getHeight() - 140, 80, 20);
    }
}

void AddFilesParameterEditor::buttonClicked(Button* button)
{
    if(button == &newVersionToggle)
    {
        repaint();
    }
    
    else if(button == &newCatButton)
    {
        for(int i = 0; i < catagoryViewer.getNumSelectedItems(); i++)
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
}
