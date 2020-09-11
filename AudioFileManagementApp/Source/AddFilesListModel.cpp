/*
  ==============================================================================

    AddFilesListModel.cpp
    Created: 10 Sep 2020 5:02:02pm
    Author:  Max Walley

  ==============================================================================
*/

#include "AddFilesListModel.h"
#include "AddFilesComponent.h"

ToggleItem::ToggleItem()
{
    addAndMakeVisible(selectButton);
    addAndMakeVisible(textLabel);
    
    selectButton.setColour(ToggleButton::tickDisabledColourId, Colours::black);
    
    selectButton.setColour(ToggleButton::tickColourId, Colours::black);
    
    textLabel.setColour(Label::textColourId, Colours::black);
}

ToggleItem::~ToggleItem()
{
    
}

void ToggleItem::resized()
{
    textLabel.setBounds(0, 0, getWidth() / 6 * 5, getHeight());
    selectButton.setBounds(getWidth() / 6 * 5, 0, getHeight(), getHeight());
}

void ToggleItem::paint(Graphics& g)
{
    
}

void ToggleItem::setItemText(const String& newText)
{
    textLabel.setText(newText, dontSendNotification);
}

void ToggleItem::addButtonListener(Button::Listener* newListener)
{
    selectButton.addListener(newListener);
}

void ToggleItem::setRowNum(int newRowNum)
{
    rowNumber = newRowNum;
}

int ToggleItem::getRowNum() const
{
    return rowNumber;
}



AddFilesListModel::AddFilesListModel(Button::Listener* itemButtonListener) : toggleButtonLis(itemButtonListener), dataset(nullptr)
{
    
}

AddFilesListModel::~AddFilesListModel()
{
    
}

int AddFilesListModel::getNumRows()
{
    if(dataset == nullptr)
    {
        return 0;
    }
    
    return dataset->size();
}

void AddFilesListModel::paintListBoxItem (int rowNumber, Graphics &g, int width, int height, bool rowIsSelected)
{
    g.drawLine(0, 0, width, 0, 2);
}

Component* AddFilesListModel::refreshComponentForRow (int rowNum, bool isRowSelected, Component* existingComponent)
{
    if(rowNum >= getNumRows())
    {
        return nullptr;
    }
    
    ToggleItem* itemToReturn = dynamic_cast<ToggleItem*>(existingComponent);
    
    if(itemToReturn == nullptr)
    {
        itemToReturn = new ToggleItem;
        
        if(toggleButtonLis != nullptr)
        {
            itemToReturn->addButtonListener(toggleButtonLis);
        }
    }
    
    itemToReturn->setRowNum(rowNum);
    
    itemToReturn->setItemText((*dataset)[rowNum].getFileName());
    
    return itemToReturn;
}

void AddFilesListModel::setDataset(Array<File>* newData)
{
    dataset = newData;
}
