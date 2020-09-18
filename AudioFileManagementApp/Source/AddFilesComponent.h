/*
  ==============================================================================

    AddFilesComponent.h
    Created: 10 Sep 2020 2:26:58pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AddFilesListModel.h"
#include "AddFilesParameterEditor.h"

//==============================================================================
/*
*/
using namespace juce;

class FileArraySorter
{
public:
    int compareElements(File& first, File& second) const;
};

class AddFilesComponent  : public Component,
                           public Button::Listener
{
public:
    AddFilesComponent(juce::ValueTree currentData);
    ~AddFilesComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    //Opens a file browser and adds the files to the component
    //Returns whether valid files were selected
    bool lookForFilesAndAdd();

private:
    
    ErrorSymbol test;
    
    void buttonClicked(Button* button) override;
    
    ValueTree newFileData;
    ListBox fileList;
    AddFilesListModel listModel;
    
    AddFilesParameterEditor paramEditor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddFilesComponent)
};
