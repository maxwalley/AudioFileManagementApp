/*
  ==============================================================================

    AddFilesListModel.h
    Created: 10 Sep 2020 5:02:02pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;
 
class ErrorSymbol : public Component
{
public:
    ErrorSymbol();
    ~ErrorSymbol();
    
    void paint(Graphics& g) override;
    
private:
};

class ListItem
{
public:
    ListItem();
    ~ListItem();
    
    void setRowNum(int newRowNum);
    int getRowNum() const;
    
private:
    int rowNumber;
};

class ToggleItem : public Component,
                   public ListItem
{
public:
    ToggleItem();
    ~ToggleItem();
    
    void resized() override;
    
    void setItemText(const String& newText);
    
    void addButtonListener(Button::Listener* newListener);
    
    bool getButtonState() const;
    void setButtonState(bool newState, NotificationType sendNotification);
    
    bool getCompleted() const;
    void setCompleted(bool newCompleted);
    
private:
    ToggleButton selectButton;
    Label textLabel;
    ErrorSymbol notCompletedSymbol;
    
    bool completed;
};


class AddFilesListModel : public ListBoxModel
{
public:
    //itemButtonListener is a listener for the ToggleButtons on the items. If this is nullptr no listener will be added when the buttons are created
    AddFilesListModel(ValueTree dataModel, Button::Listener* itemButtonListener = nullptr);
    ~AddFilesListModel();
    
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForRow (int rowNum, bool isRowSelected, Component* existingComponent) override;
    
private:
    Button::Listener* toggleButtonLis;
    ValueTree dataToDisplay;

};
