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

class ToggleItem : public Component
{
public:
    ToggleItem();
    ~ToggleItem();
    
    void resized() override;
    void paint(Graphics& g) override;
    
    void setItemText(const String& newText);
    
    void addButtonListener(Button::Listener* newListener);
    
    void setRowNum(int newRowNum);
    int getRowNum() const;
    
private:
    ToggleButton selectButton;
    Label textLabel;
    
    int rowNumber;
};


class AddFilesListModel : public ListBoxModel
{
public:
    //itemButtonListener is a listener for the ToggleButtons on the items. If this is nullptr no listener will be added when the buttons are created
    AddFilesListModel(Button::Listener* itemButtonListener = nullptr);
    ~AddFilesListModel();
    
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForRow (int rowNum, bool isRowSelected, Component* existingComponent) override;
    
    void setDataset(Array<File>* newData);
    
private:
    Button::Listener* toggleButtonLis;
    Array<File>* dataset;

};
