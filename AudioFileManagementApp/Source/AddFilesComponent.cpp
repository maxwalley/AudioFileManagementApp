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


AddFilesComponent::AddFilesComponent(juce::ValueTree currentData) : newFileData("NewFiles"), listModel(newFileData, this), paramEditor(currentData), filesDragged(false), addFilesButton("Add Files")
{
    setSize(600, 400);
    
    addAndMakeVisible(fileList);
    fileList.setModel(&listModel);
    fileList.setColour(ListBox::backgroundColourId, Colours::white);
    
    addAndMakeVisible(paramEditor);
    
    addAndMakeVisible(addFilesButton);
    addFilesButton.addListener(this);
}

AddFilesComponent::~AddFilesComponent()
{
}

void AddFilesComponent::paint (Graphics& g)
{
    g.setColour(Colours::white);
    g.fillRect(0, getHeight() - 30, getWidth() / 2, 30);
}

void AddFilesComponent::paintOverChildren (Graphics& g)
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
    fileList.setBounds(0, 0, getWidth() / 2, getHeight() - 30);
    paramEditor.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight());
    
    addFilesButton.setBounds(getWidth() / 4 - 50, getHeight() - 30, 100, 30);
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
        fileTree.setProperty("Catagories", "", nullptr);
        fileTree.setProperty("Keywords", "", nullptr);
        newFileData.addChild(fileTree, -1, nullptr);
    });
    
    fileList.updateContent();
}

void AddFilesComponent::buttonClicked(Button* button)
{
    if(button == &addFilesButton)
    {
        //If all are ready - add them to main data stream and close this window.
        //Otherwise highlight the ones which are not
    }
    
    ToggleItem* rowComponent = dynamic_cast<ToggleItem*>(button->getParentComponent());
    
    //Last row
    if(rowComponent->getRowNum() + 1 == listModel.getNumRows())
    {
        for(int i = 0; i < listModel.getNumRows(); i++)
        {
            ToggleItem* componentToChange = dynamic_cast<ToggleItem*>(fileList.getComponentForRowNumber(i));
            
            if(componentToChange != nullptr)
            {
                componentToChange->setButtonState(button->getToggleState(), dontSendNotification);
            }
        }
    }
    
    refreshFilesToShow();
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
        refreshFilesToShow();
        repaint();
    }
}

Array<ValueTree> AddFilesComponent::getSelectedItems()
{
    Array<ValueTree> selectedTrees;
    
    for(int i = 0; i < listModel.getNumRows() - 1; i++)
    {
        ToggleItem* currentItem = dynamic_cast<ToggleItem*>(fileList.getComponentForRowNumber(i));
        
        if(currentItem == nullptr)
        {
            continue;
        }
        
        if(currentItem->getButtonState())
        {
            selectedTrees.add(newFileData.getChild(i));
        }
    }
    
    return selectedTrees;
}

void AddFilesComponent::refreshFilesToShow()
{
    Array<ValueTree> selectedTrees = getSelectedItems();
    
    
    if(selectedTrees.size() == 0)
    {
        paramEditor.setDataToShow(ValueTree());
        return;
    }
    
    if(selectedTrees.size() == 1)
    {
        paramEditor.setDataToShow(selectedTrees[0]);
        return;
    }
    
    //If more than one
    ValueTree combinationOfTrees("File");
    
    ValueTree compTree = selectedTrees[0];
    
    //Iterates through the trees comparing them to the compTree
    for(int i = 1; i < selectedTrees.size(); i++)
    {
        if(!selectedTrees[i].getProperty("Catagories").equalsWithSameType(compTree.getProperty("Catagories")))
        {
            combinationOfTrees.setProperty("Catagories", "", nullptr);
        }
        
        if(!selectedTrees[i].getProperty("Keywords").equalsWithSameType(compTree.getProperty("Keywords")))
        {
            combinationOfTrees.setProperty("Keywords", "Multiple Values", nullptr);
        }
    }
    
    //If no differences have been found it sets this to the comp value
    if(!combinationOfTrees.hasProperty("Catagories"))
    {
        combinationOfTrees.setProperty("Catagories", compTree.getProperty("Catagories"), nullptr);
    }
    
    if(!combinationOfTrees.hasProperty("Keywords"))
    {
        combinationOfTrees.setProperty("Keywords", compTree.getProperty("Keywords"), nullptr);
    }
    
    paramEditor.setDataToShow(combinationOfTrees);
}
