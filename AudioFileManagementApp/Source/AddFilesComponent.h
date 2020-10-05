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
                           public FileDragAndDropTarget,
                           public AddFilesParameterEditor::Listener
{
public:
    
    enum class KeywordType
    {
        noun,
        verb,
        other
    };
    
    AddFilesComponent(ValueTree currentData);
    ~AddFilesComponent() override;

    void paint(Graphics& g) override;
    void paintOverChildren (Graphics& g) override;
    void resized() override;
    
    bool processAndAddFiles(const Array<File>& filesToAdd);
    void addFiles(const Array<File>& filesToAdd);

private:
    
    void buttonClicked(Button* button) override;
    
    bool isInterestedInFileDrag(const StringArray& files) override;
    
    void filesDropped(const StringArray& files, int x, int y) override;
    
    void fileDragEnter(const StringArray& files, int x, int y) override;
    
    void fileDragExit(const StringArray& files) override;
    
    void fileDragMove(const StringArray& files, int x, int y) override;
    
    void dataChanged(AddFilesParameterEditor::KeywordType specificDataFieldChanged) override;
    
    Array<ValueTree> getSelectedItems();
    
    void refreshFilesToShow();
    
    //Returns false if there is difference true if not
    bool compareTreeNoOrder(const ValueTree& first, const ValueTree& second) const;
    
    std::vector<ValueTree> getChildrenWithName(const ValueTree& tree, const Identifier& name) const;
    
    int getNumChildrenWithName(const ValueTree& tree, const Identifier& name) const;
    
    //-1 if it cant find one, treeToSearch should be a keyword tree
    int getFirstIndexOfKeywordType(const ValueTree& treeToSearch, KeywordType wordTypeToLookFor) const;
    
    //-1 if it cant find one, treeToSearch should be a keyword tree
    int getLastIndexOfKeywordType(const ValueTree& treeToSearch, KeywordType wordTypeToLookFor) const;
    
    ValueTree fxData;
    ValueTree newFileData;
    ListBox fileList;
    AddFilesListModel listModel;
    
    AddFilesParameterEditor paramEditor;
    
    bool filesDragged;
    
    TextButton addFilesButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddFilesComponent)
};
