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


AddFilesComponent::AddFilesComponent(juce::ValueTree currentData, FXDataFormatter* dataFormatter) : dataToAddTo(currentData), newFileData("NewFiles"), listModel(newFileData, this), paramEditor(currentData.getChildWithName("Categories")), filesDragged(false), addFilesButton("Add Files")
{
    setSize(600, 400);
    
    addAndMakeVisible(fileList);
    fileList.setModel(&listModel);
    fileList.setColour(ListBox::backgroundColourId, Colours::white);
    
    addAndMakeVisible(paramEditor);
    paramEditor.addListener(this);
    
    addAndMakeVisible(addFilesButton);
    addFilesButton.addListener(this);
    
    dataToAddTo.addListener(this);
    
    if(dataFormatter != nullptr)
    {
        newFileData.addListener(dataFormatter);
    }
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
        ValueTree fileTree("FX");
        fileTree.setProperty("Path", file.getFullPathName(), nullptr);
        
        ValueTree listBoxDisplayData("ListBoxData");
        listBoxDisplayData.setProperty("Selected", false, nullptr);
        listBoxDisplayData.setProperty("Completed", false, nullptr);
        
        fileTree.appendChild(listBoxDisplayData, nullptr);
        
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
        
        //Also all the categories that have been assigned need to have specific ID number attached to them
        //Also needs to remove the listbox display child from each
    }
    
    ToggleItem* rowComponent = dynamic_cast<ToggleItem*>(button->getParentComponent());
    
    if(rowComponent->getRowNum() < newFileData.getNumChildren())
    {
        newFileData.getChild(rowComponent->getRowNum()).getChildWithName("ListBoxData").setProperty("Selected", button->getToggleState(), nullptr);
    }
    
    //Last row
    else
    {
        std::for_each(newFileData.begin(), newFileData.end(), [button](const ValueTree& fxTree)
        {
            fxTree.getChildWithName("ListBoxData").setProperty("Selected", button->getToggleState(), nullptr);
        });
    }
    
    fileList.updateContent();
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

void AddFilesComponent::dataChanged(AddFilesParameterEditor::KeywordType specificDataFieldChanged)
{
    StringArray changedWords = paramEditor.getTextFromComponent(specificDataFieldChanged);
    
    //checks if data is ready and changes the ready symbol on the selected items
    for(int i = 0; i < newFileData.getNumChildren(); i++)
    {
        ValueTree listBoxData = newFileData.getChild(i).getChildWithName("ListBoxData");
        ValueTree keywordsTree = newFileData.getChild(i).getChildWithName("Keywords");
        
        if(listBoxData.getProperty("Selected"))
        {
            int firstIndex = getFirstIndexOfKeywordType(keywordsTree, specificDataFieldChanged);
            int lastIndex = getLastIndexOfKeywordType(keywordsTree, specificDataFieldChanged);
            
            if(changedWords.size() <= lastIndex - firstIndex)
            {
                for(int i = changedWords.size() + firstIndex; i <= lastIndex; i++)
                {
                    keywordsTree.removeChild(i, nullptr);
                }
            }
            
            for(int i = 0; i < changedWords.size(); i++)
            {
                int indexRange = lastIndex - firstIndex;
                
                if(i > indexRange || firstIndex == -1)
                {
                    ValueTree newWord("Word");
                    if(specificDataFieldChanged == AddFilesParameterEditor::KeywordType::noun)
                    {
                        newWord.setProperty("Type", "Noun", nullptr);
                    }
                    else if(specificDataFieldChanged == AddFilesParameterEditor::KeywordType::verb)
                    {
                        newWord.setProperty("Type", "Verb", nullptr);
                    }
                    
                    newWord.setProperty("Name", changedWords[i], nullptr);
                
                    keywordsTree.addChild(newWord, ++lastIndex, nullptr);
                }
                
                else if(i <= indexRange)
                {
                    //If one of this type already is there
                    if(keywordsTree.getChild(i).getProperty("Name") != changedWords[i])
                    {
                        keywordsTree.getChild(i).setProperty("Name", changedWords[i], nullptr);
                    }
                }
            }
            checkAndUpdateIfFXIsReady(newFileData.getChild(i));
        }
    }
    fileList.updateContent();
}

void AddFilesComponent::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
    if(property.toString() == "AddComponentTreeViewSelected")
    {
        //If a category has been added
        if(treeWhosePropertyHasChanged.getProperty(property))
        {
            std::for_each(newFileData.begin(), newFileData.end(), [this, &treeWhosePropertyHasChanged](ValueTree fxTree)
            {
                if(fxTree.getChildWithName("ListBoxData").getProperty("Selected"))
                {
                    var categoryID = treeWhosePropertyHasChanged.getProperty("IDNum");
                    var fxID = fxTree.getProperty("IDNum");
                
                    //Adds the category ID number to the FX
                    ValueTree newCategory("Category");
                    newCategory.setProperty("IDNum", categoryID, nullptr);
                    fxTree.getChildWithName("Categories").appendChild(newCategory, nullptr);
                
                    //Adds the FX ID number to the category
                    ValueTree newFX("FX");
                    newFX.setProperty("IDNum", fxID, nullptr);
                    treeWhosePropertyHasChanged.getChildWithName("FXList").appendChild(newFX, nullptr);
                
                    checkAndUpdateIfFXIsReady(fxTree);
                }
            });
        }
        
        //If a category has been deselected
        else
        {
            std::for_each(newFileData.begin(), newFileData.end(), [this, &treeWhosePropertyHasChanged](ValueTree fxTree)
            {
                if(fxTree.getChildWithName("ListBoxData").getProperty("Selected"))
                {
                    var categoryID = treeWhosePropertyHasChanged.getProperty("IDNum");
                    var fxID = fxTree.getProperty("IDNum");
                    
                    //Removes the category ID from the FX
                    ValueTree categoryToRemove = fxTree.getChildWithName("Categories").getChildWithProperty("IDNum", categoryID);
                    fxTree.getChildWithName("Categories").removeChild(categoryToRemove, nullptr);
                    
                    //Removes the FX ID from the category
                    ValueTree FXToRemove = treeWhosePropertyHasChanged.getChildWithName("FXList").getChildWithProperty("IDNum", fxID);
                    treeWhosePropertyHasChanged.getChildWithName("FXList").removeChild(FXToRemove, nullptr);
                    
                    checkAndUpdateIfFXIsReady(fxTree);
                }
            });
        }
    }
}

Array<ValueTree> AddFilesComponent::getSelectedItems()
{
    Array<ValueTree> selectedTrees;
    
    for(int i = 0; i < newFileData.getNumChildren(); i++)
    {
        if(newFileData.getChild(i).getChildWithName("ListBoxData").getProperty("Selected"))
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
    
    //Whether there are differences in categories in selected values
    bool catDif = false;
    
    //Whether there are differences in keywords in selected values
    bool keyDif = false;
    
    //Iterates through the trees comparing them to the compTree
    for(int i = 1; i < selectedTrees.size() && (!catDif || !keyDif); i++)
    {
        if(!catDif)
        {
            if(compareTreeNoOrder(compTree.getChildWithName("Categories"), selectedTrees[i].getChildWithName("Categories")) == 0)
            {
                catDif = true;
            }
        }
        
        if(!keyDif)
        {
            if(compareTreeNoOrder(compTree.getChildWithName("Keywords"), selectedTrees[i].getChildWithName("Keywords")) == 0)
            {
                keyDif = true;
            }
        }
    }
    
    ValueTree combCats("Categories");
    
    //If differences have been found it sets this to the comp value
    if(catDif)
    {
        ValueTree multipleCats("Category");
        multipleCats.setProperty("Name", "Multiple Values", nullptr);
        combCats.appendChild(multipleCats, nullptr);
    }
    else
    {
        combCats = compTree.getChildWithName("Categories").createCopy();
    }
    
    combinationOfTrees.appendChild(combCats, nullptr);
    
    ValueTree combKeys("Keywords");
    
    //If differences have been found it sets this to the comp value
    if(keyDif)
    {
        ValueTree multipleKeys("Keywords");
        multipleKeys.setProperty("Name", "Multiple Values", nullptr);
        combKeys.appendChild(multipleKeys, nullptr);
    }
    else
    {
        combKeys = compTree.getChildWithName("Keywords").createCopy();
    }
    
    combinationOfTrees.appendChild(combKeys, nullptr);
    
    paramEditor.setDataToShow(combinationOfTrees);
}

bool AddFilesComponent::compareTreeNoOrder(const ValueTree& first, const ValueTree& second) const
{
    if(first.getNumChildren() != second.getNumChildren())
    {
        return false;
    }
    
    //This checks this tree to see if the numbers of trees with the same name in it are the same
    std::vector<Identifier> checkedIds;
    
    for(const ValueTree& tree : first)
    {
        Identifier type = tree.getType();
        
        //If the ID of this tree has not been checked this checks that this tree and the second tree have the same number of this ID
        if(checkedIds.empty() || std::find(checkedIds.begin(), checkedIds.end(), type) == checkedIds.end())
        {
            int numInFirst = getNumChildrenWithName(first, type);
            int numInSecond = getNumChildrenWithName(second, type);
            
            if(numInFirst != numInSecond)
            {
                return false;
            }
            
            if(numInFirst > 1)
            {
                checkedIds.push_back(type);
            }
        }
        
        //From the second tree we take all the children with the same ID as this one
        std::vector<ValueTree> itemsToComp = getChildrenWithName(second, type);
            
        bool foundMatch = false;
            
        //Iterate through all these children until we find one that matches the one we're looking for
        for(int i = 0; i < itemsToComp.size(); i++)
        {
            if(itemsToComp[i].hasProperty("check"))
            {
                continue;
            }
                
            if (compareTreeNoOrder(tree, itemsToComp[i]) == true)
            {
                //We mark the one that has been matched with so it cant be matched with again
                itemsToComp[i].setProperty("check", true, nullptr);
                foundMatch = true;
            }
        }
           
        //If a match wasn't found these trees do not have the same children
        if(!foundMatch)
        {
            return false;
        }
    }
    
    std::for_each(second.begin(), second.end(), [](ValueTree curTree)
    {
        curTree.removeProperty("check", nullptr);
    });
    
    return true;
}

std::vector<ValueTree> AddFilesComponent::getChildrenWithName(const ValueTree& tree, const Identifier& name) const
{
    std::vector<ValueTree> trees;
    
    std::for_each(tree.begin(), tree.end(), [&name, &trees](const ValueTree& searchTree)
    {
        if(searchTree.getType() == name)
        {
            trees.push_back(searchTree);
        }
    });
    
    return trees;
}

int AddFilesComponent::getNumChildrenWithName(const ValueTree& tree, const Identifier& name) const
{
    int count = 0;
    
    std::for_each(tree.begin(), tree.end(), [&name, &count](const ValueTree& selTree)
    {
        if(selTree.getType() == name)
        {
            count++;
        }
    });
    
    return count;
}

int AddFilesComponent::getFirstIndexOfKeywordType(const ValueTree& treeToSearch, AddFilesParameterEditor::KeywordType wordTypeToLookFor) const
{
    if(wordTypeToLookFor == AddFilesParameterEditor::KeywordType::noun)
    {
        if(treeToSearch.getChild(0).getProperty("Type") != "Noun")
        {
            return -1;
        }
        
        return 0;
    }
    
    else if(wordTypeToLookFor == AddFilesParameterEditor::KeywordType::verb)
    {
        for(int i = 0; i < treeToSearch.getNumChildren(); i++)
        {
            if(treeToSearch.getChild(i).getProperty("Type") == "Verb")
            {
                return i;
            }
            else if(!treeToSearch.getChild(i).hasProperty("Type"))
            {
                return -1;
            }
        }
        return -1;
    }
    
    for(int i = 0; i < treeToSearch.getNumChildren(); i++)
    {
        if(!treeToSearch.getChild(i).hasProperty("Type"))
        {
            return i;
        }
    }
    return -1;
}

int AddFilesComponent::getLastIndexOfKeywordType(const ValueTree& treeToSearch, AddFilesParameterEditor::KeywordType wordTypeToLookFor) const
{
    if(wordTypeToLookFor == AddFilesParameterEditor::KeywordType::other)
    {
        if(treeToSearch.getChild(treeToSearch.getNumChildren() - 1).hasProperty("Type"))
        {
            return -1;
        }
        return treeToSearch.getNumChildren() - 1;
    }
    
    int firstIndex = getFirstIndexOfKeywordType(treeToSearch, wordTypeToLookFor);
    
    if(firstIndex == -1)
    {
        return -1;
    }
    
    String wordToLookFor;
    
    switch (wordTypeToLookFor)
    {
        case AddFilesParameterEditor::KeywordType::noun:
            wordToLookFor = "Noun";
            break;
            
        case AddFilesParameterEditor::KeywordType::verb:
            wordToLookFor = "Verb";
            break;
    }
    
    for(int i = firstIndex; i < treeToSearch.getNumChildren(); i++)
    {
        if(treeToSearch.getChild(i).getProperty("Type") == wordToLookFor && (treeToSearch.getChild(i+1).getProperty("Type") != wordToLookFor || !treeToSearch.getChild(i+1).isValid()))
        {
            return i;
        }
    }
    
    return -1;
}

void AddFilesComponent::checkAndUpdateIfFXIsReady(ValueTree treeToCheck)
{
    ValueTree keywordsTree = treeToCheck.getChildWithName("Keywords");
    ValueTree categoriesTree = treeToCheck.getChildWithName("Categories");
    ValueTree listBoxDataTree = treeToCheck.getChildWithName("ListBoxData");
    
    std::for_each(keywordsTree.begin(), keywordsTree.end(), [](const ValueTree& tree)
    {
        //DBG(tree.getProperty("Name").toString());
    });
    
    //Checks and sets whether the data is ready
    if(keywordsTree.getNumChildren() == 0 || categoriesTree.getNumChildren() == 0)
    {
        listBoxDataTree.setProperty("Completed", false, nullptr);
    }
    else
    {
        listBoxDataTree.setProperty("Completed", true, nullptr);
    }
}
