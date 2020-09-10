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
    
private:
    ToggleButton selectButton;
    Label textLabel;
};


class AddFilesListModel : public ListBoxModel
{
public:
    AddFilesListModel();
    ~AddFilesListModel();
    
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, Graphics &g, int width, int height, bool rowIsSelected) override;
    Component* refreshComponentForRow (int rowNum, bool isRowSelected, Component* existingComponent) override;
    
    void setDataset(Array<File>* newData);
    
private:
    Array<File>* dataset;

};
