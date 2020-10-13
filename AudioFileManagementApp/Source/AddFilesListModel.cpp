/*
  ==============================================================================

    AddFilesListModel.cpp
    Created: 10 Sep 2020 5:02:02pm
    Author:  Max Walley

  ==============================================================================
*/

#include "AddFilesListModel.h"
#include "AddFilesComponent.h"

ErrorSymbol::ErrorSymbol()
{
    
}

ErrorSymbol::~ErrorSymbol()
{
    
}

void ErrorSymbol::paint(Graphics& g)
{
    Path outline;
    
    outline.addTriangle(Point<float>(0, getHeight()), Point<float>(getWidth() / 2, 0), Point<float>(getWidth(), getHeight()));
    
    outline = outline.createPathWithRoundedCorners(4.0);
    
    g.setColour(Colours::red);
    
    g.fillPath(outline);
    
    g.setColour(Colours::white);
    
    float width = float(getWidth());
    float height = float(getHeight());
    
    float rectWidth = width / 9.0f;
    float circWidth = width / 7.0f;
    
    g.fillRect((width / 2.0f - 1.0f), height / 3.25f, rectWidth, height / 3.0f);
    
    g.fillEllipse((width / 2.0f - 1.0f), height / 4.0f * 3.0f, circWidth, height / 7.0f);
}


ListItem::ListItem()
{
    
}

ListItem::~ListItem()
{
    
}

void ListItem::setRowNum(int newRowNum)
{
    rowNumber = newRowNum;
}

int ListItem::getRowNum() const
{
    return rowNumber;
}


ToggleItem::ToggleItem() : completed(false)
{
    addAndMakeVisible(selectButton);
    addAndMakeVisible(textLabel);
    
    selectButton.setColour(ToggleButton::tickDisabledColourId, Colours::black);
    
    selectButton.setColour(ToggleButton::tickColourId, Colours::black);
    
    textLabel.setColour(Label::textColourId, Colours::black);
    
    addAndMakeVisible(notCompletedSymbol);
}

ToggleItem::~ToggleItem()
{
    
}

void ToggleItem::resized()
{
    textLabel.setBounds(getHeight(), 0, getWidth() - (getHeight() * 2), getHeight());
    selectButton.setBounds(getWidth() - getHeight(), 0, getHeight(), getHeight());
    
    notCompletedSymbol.setVisible(!completed);
    
    if(!completed)
    {
        notCompletedSymbol.setBounds(2, 2, getHeight() - 4, getHeight() - 4);
    }
}

void ToggleItem::setItemText(const String& newText)
{
    textLabel.setText(newText, dontSendNotification);
}

void ToggleItem::addButtonListener(Button::Listener* newListener)
{
    selectButton.addListener(newListener);
}

bool ToggleItem::getButtonState() const
{
    return selectButton.getToggleState();
}

void ToggleItem::setButtonState(bool newState, NotificationType sendNotification)
{
    selectButton.setToggleState(newState, dontSendNotification);
}

bool ToggleItem::getCompleted() const
{
    return completed;
}

void ToggleItem::setCompleted(bool newCompleted)
{
    completed = newCompleted;
    resized();
}

AddFilesListModel::AddFilesListModel(ValueTree dataModel, Button::Listener* itemButtonListener) : toggleButtonLis(itemButtonListener), dataToDisplay(dataModel)
{
    
}

AddFilesListModel::~AddFilesListModel()
{
    
}

int AddFilesListModel::getNumRows()
{
    int numFiles = dataToDisplay.getNumChildren();
    
    if(numFiles > 0)
    {
        return ++numFiles;
    }
    return 0;
}

void AddFilesListModel::paintListBoxItem (int rowNumber, Graphics &g, int width, int height, bool rowIsSelected)
{
    g.drawLine(0, 0, width, 0, 2);
}

Component* AddFilesListModel::refreshComponentForRow (int rowNum, bool isRowSelected, Component* existingComponent)
{
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
    
    ValueTree currentTree = dataToDisplay.getChild(rowNum);
    
    if(rowNum < getNumRows() - 1)
    {
        String fileName = currentTree.getProperty("Path");
        
        fileName = fileName.fromLastOccurrenceOf("/", false, false);
        itemToReturn->setItemText(fileName);
        itemToReturn->setCompleted(currentTree.getChildWithName("ListBoxData").getProperty("Completed"));
        itemToReturn->setButtonState(currentTree.getChildWithName("ListBoxData").getProperty("Selected"), sendNotification);
    }
    //Last row
    else
    {
        itemToReturn->setItemText("All Items");
        itemToReturn->setCompleted(true);
    }
    
    return itemToReturn;
}
