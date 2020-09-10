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
AddFilesComponent::AddFilesComponent()
{
    setSize(600, 400);
}

AddFilesComponent::~AddFilesComponent()
{
}

void AddFilesComponent::paint (juce::Graphics& g)
{
    
}

void AddFilesComponent::resized()
{
    
}

bool AddFilesComponent::lookForFilesAndAdd()
{
    FileChooser chooser("Select files to add", File::getSpecialLocation(File::userDocumentsDirectory));
    
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
        
        return true;
    }
    return false;
}
