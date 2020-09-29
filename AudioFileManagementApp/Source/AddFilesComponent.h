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
                           public Button::Listener,
                           public FileDragAndDropTarget
{
public:
    AddFilesComponent(ValueTree currentData);
    ~AddFilesComponent() override;

    void paintOverChildren (juce::Graphics&) override;
    void resized() override;
    
    bool processAndAddFiles(const Array<File>& filesToAdd);
    void addFiles(const Array<File>& filesToAdd);

private:
    
    ErrorSymbol test;
    
    void buttonClicked(Button* button) override;
    
    bool isInterestedInFileDrag(const StringArray& files) override;
    
    void filesDropped(const StringArray& files, int x, int y) override;
    
    void fileDragEnter(const StringArray& files, int x, int y) override;
    
    void fileDragExit(const StringArray& files) override;
    
    void fileDragMove(const StringArray& files, int x, int y) override;
    
    ValueTree newFileData;
    ListBox fileList;
    AddFilesListModel listModel;
    
    AddFilesParameterEditor paramEditor;
    
    bool filesDragged;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddFilesComponent)
};
