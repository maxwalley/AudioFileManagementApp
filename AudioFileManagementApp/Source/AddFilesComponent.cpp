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


AddFilesComponent::AddFilesComponent(juce::ValueTree currentData) : newFileData("NewFiles"), listModel(newFileData, this), paramEditor(currentData), filesDragged(false)
{
    setSize(600, 400);
    
    addAndMakeVisible(fileList);
    
    fileList.setModel(&listModel);
    
    fileList.setColour(ListBox::backgroundColourId, Colours::white);
    
    addAndMakeVisible(test);
    
    addAndMakeVisible(paramEditor);
}

AddFilesComponent::~AddFilesComponent()
{
}

void AddFilesComponent::paintOverChildren (juce::Graphics& g)
{
    if(filesDragged)
    {
        g.setColour(Colour::fromFloatRGBA(192.0, 192.0, 192.0, 0.7));
        g.fillRect(0, 0, getWidth() / 2, getHeight());
        
        g.setColour(Colours::black);
        g.setFont(Font(18));
        g.drawText("Drop Files To Add", 0, getHeight() / 2, getWidth() / 2, 18, Justification::centred | Justification::verticallyCentred);
    }
}

void AddFilesComponent::resized()
{
    fileList.setBounds(0, 0, getWidth() / 2, getHeight());
    paramEditor.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight());
}

bool AddFilesComponent::processAndAddFiles(const Array<File>& filesToAdd)
{
    Array<File> newFiles = filesToAdd;
    
    for(int i = 0; i < newFiles.size(); i++)
    {
        //Unpacks any directories
        if(newFiles[i].isDirectory())
        {
            Array<File> directoryFiles = newFiles[i].findChildFiles(File::findFiles, true);
            
            newFiles.insertArray(i + 1, directoryFiles.data(), directoryFiles.size());
            
            newFiles.remove(i);
            
            i--;
        }
        
        //Unpacks a zip file
        else if(newFiles[i].getFileExtension() == ".zip")
        {
            ZipFile zip(newFiles[i]);
            
            File newFolderToUnzipTo(filesToAdd[i].getFullPathName().trimCharactersAtEnd(".zip"));
            
            zip.uncompressTo(newFolderToUnzipTo);
            
            Array<File> directoryFiles = newFolderToUnzipTo.findChildFiles(File::findFiles, true);
            
            newFiles.insertArray(i + 1, directoryFiles.data(), directoryFiles.size());
            
            newFiles.remove(i);
            
            i--;
        }
    }
    
    if(newFiles.isEmpty())
    {
        return false;
    }
    
    FileArraySorter sorter;
    newFiles.sort(sorter);
    
    addFiles(newFiles);
    
    return true;
}

void AddFilesComponent::addFiles(const juce::Array<File>& filesToAdd)
{
    std::for_each(filesToAdd.begin(), filesToAdd.end(), [this](const File& file)
    {
        ValueTree fileTree("File");
        fileTree.setProperty("Path", file.getFullPathName(), nullptr);
        newFileData.addChild(fileTree, -1, nullptr);
    });
    
    fileList.updateContent();
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

bool AddFilesComponent::isInterestedInFileDrag(const StringArray& files)
{
    return true;
}

void AddFilesComponent::filesDropped(const StringArray& files, int x, int y)
{
    if(x <= getWidth() / 2)
    {
        Array<File> filesToAdd;
        
        std::for_each(files.begin(), files.end(), [&filesToAdd](const String& file)
        {
            filesToAdd.add(File(file));
        });
        
        processAndAddFiles(filesToAdd);
        
        filesDragged = false;
        repaint();
    }
}

void AddFilesComponent::fileDragEnter(const StringArray& files, int x, int y)
{
    if(x <= getWidth() / 2 && !filesDragged)
    {
        filesDragged = true;
        repaint();
    }
}

void AddFilesComponent::fileDragExit(const StringArray& files)
{
    if(filesDragged)
    {
        filesDragged = false;
        repaint();
    }
}

void AddFilesComponent::fileDragMove(const StringArray& files, int x, int y)
{
    if(x > getWidth() / 2 && filesDragged)
    {
        filesDragged = false;
        repaint();
    }
    
    else if(x <= getWidth() / 2 && !filesDragged)
    {
        filesDragged = true;
        repaint();
    }
}
