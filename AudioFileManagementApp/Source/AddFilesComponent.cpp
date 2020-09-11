/*
  ==============================================================================

    AddFilesComponent.cpp
    Created: 10 Sep 2020 2:26:58pm
    Author:  Max Walley

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AddFilesComponent.h"

//==============================================================================
AddFilesComponent::AddFilesComponent() : listModel(this)
{
    setSize(600, 400);
    
    addAndMakeVisible(fileList);
    
    fileList.setModel(&listModel);
    
    fileList.setColour(ListBox::backgroundColourId, Colours::white);
}

AddFilesComponent::~AddFilesComponent()
{
}

void AddFilesComponent::paint (juce::Graphics& g)
{
    
}

void AddFilesComponent::resized()
{
    fileList.setBounds(0, 0, getWidth() / 3, getHeight());
}

bool AddFilesComponent::lookForFilesAndAdd()
{
    filesToAdd.clear();
    
    FileChooser chooser("Select files to add");
    
    if(chooser.browseForMultipleFilesOrDirectories())
    {
        filesToAdd = chooser.getResults();
        
        for(int i = 0; i < filesToAdd.size(); i++)
        {
            //Unpacks any directories
            if(filesToAdd[i].isDirectory())
            {
                Array<File> directoryFiles = filesToAdd[i].findChildFiles(File::findFiles, true);
                
                filesToAdd.insertArray(i+1, directoryFiles.data(), directoryFiles.size());
                
                filesToAdd.remove(i);
            }
        }
        
        if(filesToAdd.isEmpty())
        {
            return false;
        }
        
        listModel.setDataset(&filesToAdd);
        fileList.updateContent();
        
        return true;
    }
    return false;
}

void AddFilesComponent::buttonClicked(Button* button)
{
    ToggleItem* rowComponent = dynamic_cast<ToggleItem*>(button->getParentComponent());
    
    DBG(rowComponent->getRowNum());    
}
