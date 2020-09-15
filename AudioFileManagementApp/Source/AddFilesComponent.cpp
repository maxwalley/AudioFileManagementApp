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

int FileArraySorter::compareElements(File& first, File& second) const
{
    //If in the same directory
    if(first.getParentDirectory() == second.getParentDirectory())
    {
        return first.getFileName().compareIgnoreCase(second.getFileName());
    }
    
    return first.getParentDirectory().getFileName().compareIgnoreCase(second.getParentDirectory().getFileName());
}


AddFilesComponent::AddFilesComponent() : newFileData("NewFiles"), listModel(newFileData, this)
{
    setSize(600, 400);
    
    addAndMakeVisible(fileList);
    
    fileList.setModel(&listModel);
    
    fileList.setColour(ListBox::backgroundColourId, Colours::white);
    
    addAndMakeVisible(test);
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
    Array<File> filesToAdd;
    
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
                
                filesToAdd.insertArray(i + 1, directoryFiles.data(), directoryFiles.size());
                
                filesToAdd.remove(i);
                
                i--;
            }
            
            //Unpacks a zip file
            else if(filesToAdd[i].getFileExtension() == ".zip")
            {
                ZipFile zip(filesToAdd[i]);
                
                File newFolderToUnzipTo(filesToAdd[i].getFullPathName().trimCharactersAtEnd(".zip"));
                
                zip.uncompressTo(newFolderToUnzipTo);
                
                Array<File> directoryFiles = newFolderToUnzipTo.findChildFiles(File::findFiles, true);
                
                filesToAdd.insertArray(i + 1, directoryFiles.data(), directoryFiles.size());
                
                filesToAdd.remove(i);
                
                i--;
            }
        }
        
        if(filesToAdd.isEmpty())
        {
            return false;
        }
        
        FileArraySorter arraySorter;
        filesToAdd.sort(arraySorter);
        
        //Move this data into a value tree
        //Have the model work from the valueTree
        
        std::for_each(filesToAdd.begin(), filesToAdd.end(), [this](const File& file)
        {
            ValueTree fileTree("File");
            fileTree.setProperty("Path", file.getFullPathName(), nullptr);
            newFileData.addChild(fileTree, -1, nullptr);
        });
        
        fileList.updateContent();
        
        return true;
    }
    return false;
}

void AddFilesComponent::buttonClicked(Button* button)
{
    ToggleItem* rowComponent = dynamic_cast<ToggleItem*>(button->getParentComponent());
    
    //Last row
    if(rowComponent->getRowNum() + 1 == listModel.getNumRows())
    {
        for(int i = 0; i < listModel.getNumRows(); i++)
        {
            ToggleItem* componentToChange = dynamic_cast<ToggleItem*>(fileList.getComponentForRowNumber(i));
            
            if(componentToChange != nullptr)
            {
                componentToChange->setButtonState(button->getToggleState(), sendNotification);
            }
        }
    }
}
