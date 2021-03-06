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


AddFilesComponent::AddFilesComponent(juce::ValueTree currentData) : dataToAddTo(currentData), newFileData(currentData.getChildWithName("FXList").getChildWithName("NewFiles")), listModel(newFileData, this), paramEditor(currentData.getChildWithName("Categories")), filesDragged(false), addFilesButton("Add Selected Files"), removeFilesButton("Remove Selected Files")
{
    setSize(600, 400);
    
    addAndMakeVisible(fileList);
    fileList.setModel(&listModel);
    fileList.setColour(ListBox::backgroundColourId, Colours::white);
    
    addAndMakeVisible(paramEditor);
    paramEditor.addListener(this);
    
    addAndMakeVisible(addFilesButton);
    addFilesButton.addListener(this);
    
    addAndMakeVisible(removeFilesButton);
    removeFilesButton.addListener(this);
    
    dataToAddTo.addListener(this);
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
    
    addFilesButton.setBounds(20, getHeight() - 30, 120, 30);
    
    removeFilesButton.setBounds(160, getHeight() - 30, 120, 30);
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
    
    if(addFiles(newFiles) == 0)
    {
        return false;
    }
    
    return true;
}

int AddFilesComponent::addFiles(const juce::Array<File>& filesToAdd)
{
    int numFilesAdded = 0;
    
    for(const File& file : filesToAdd)
    {
        const String path = file.getFullPathName();
        
        if(checkIfFileAlreadyExistsInDatabase(path))
        {
            if(!AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, "File Already Exists in DataBase", file.getFileName() + " already exists in the database are you sure you want to add it again?", "Add", "Do Not Add", this))
            {
                continue;
            }
        }
        
        //Add the trees then set properties so that we get the callbacks
        ValueTree fileTree("FX");
        
        ValueTree listBoxDisplayData("ListBoxData");
        
        fileTree.appendChild(listBoxDisplayData, nullptr);
        
        newFileData.addChild(fileTree, -1, nullptr);
        
        fileTree.setProperty("Path", path, nullptr);
        listBoxDisplayData.setProperty("Selected", false, nullptr);
        listBoxDisplayData.setProperty("Completed", false, nullptr);
        
        numFilesAdded++;
    }
    
    fileList.updateContent();
    
    return numFilesAdded;
}

int AddFilesComponent::getNumberOfFilesBeingAdded() const
{
    return newFileData.getNumChildren();
}

void AddFilesComponent::addListener(Listener* newListener)
{
    listeners.push_back(newListener);
}

void AddFilesComponent::removeListener(Listener* listenerToRemove)
{
    std::remove(listeners.begin(), listeners.end(), listenerToRemove);
}

void AddFilesComponent::buttonClicked(Button* button)
{
    if(button == &addFilesButton)
    {
        bool anySelectedAndComplete = false;
        int oldNumFilesToAdd = newFileData.getNumChildren();
        
        for (int i = 0; i < newFileData.getNumChildren(); i++)
        {
            ValueTree file = newFileData.getChild(i);
            ValueTree listBoxData = file.getChildWithName("ListBoxData");
            
            if(listBoxData.getProperty("Selected") && listBoxData.getProperty("Completed"))
            {
                anySelectedAndComplete = true;
                
                file.removeChild(listBoxData, nullptr);
                
                file.setProperty("Moving", true, nullptr);
                
                newFileData.removeChild(file, nullptr);
                dataToAddTo.getChildWithName("FXList").appendChild(file, nullptr);
                
                i--;
            }
        }
        
        if(!anySelectedAndComplete)
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon, "No Selected Items are Ready to be Added!", "None of the selected items are ready to be added to this application. To make an item ready it must have at least one category selected and one keyword entered. If an item is not ready an alert icon is displayed on said items row.", "Close", this);
            return;
        }
        
        std::for_each(listeners.begin(), listeners.end(), [this, &oldNumFilesToAdd](Listener* lis)
        {
            lis->filesAdded(newFileData.getNumChildren() - oldNumFilesToAdd);
        });
        
        fileList.updateContent();
        paramEditor.setDataToShow(ValueTree());
        
        return;
    }
    
    else if(button == &removeFilesButton)
    {
        for(int i = 0; i < newFileData.getNumChildren(); i++)
        {
            ValueTree listBoxData = newFileData.getChild(i).getChildWithName("ListBoxData");
            
            if(listBoxData.getProperty("Selected"))
            {
                newFileData.removeChild(i, nullptr);
            }
        }
        
        fileList.updateContent();
        paramEditor.setDataToShow(ValueTree());
        
        return;
    }
    
    //One of the toggle buttons from the listBox
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
    
    var typeToLookFor;
    
    switch (specificDataFieldChanged)
    {
        case AddFilesParameterEditor::KeywordType::noun:
            typeToLookFor = "Noun";
            break;
            
        case AddFilesParameterEditor::KeywordType::verb:
            typeToLookFor = "Verb";
            break;
            
        case AddFilesParameterEditor::KeywordType::other:
            typeToLookFor = "Unspec";
            break;
    }
    
    std::for_each(newFileData.begin(), newFileData.end(), [this, &changedWords, &specificDataFieldChanged, &typeToLookFor](ValueTree fxTree)
    {
        if(fxTree.getChildWithName("ListBoxData").getProperty("Selected"))
        {
            ValueTree keywordsTree = fxTree.getChildWithName("Keywords");
            int startIndex = getFirstIndexOfKeywordType(keywordsTree, specificDataFieldChanged);
            int endIndex = getLastIndexOfKeywordType(keywordsTree, specificDataFieldChanged);
            
            if(startIndex == -1)
            {
                if(specificDataFieldChanged == AddFilesParameterEditor::KeywordType::noun)
                {
                    startIndex = 0;
                }
                else if(specificDataFieldChanged == AddFilesParameterEditor::KeywordType::verb)
                {
                    int otherIndex = getFirstIndexOfKeywordType(keywordsTree, AddFilesParameterEditor::KeywordType::other);
                    
                    if(otherIndex != -1)
                    {
                        startIndex = otherIndex;
                    }
                    else
                    {
                        int numKeys = keywordsTree.getNumChildren();
                        
                        if(numKeys == 0)
                        {
                            startIndex = 0;
                        }
                        else
                        {
                            startIndex = numKeys;
                        }
                    }
                }
                else if(specificDataFieldChanged == AddFilesParameterEditor::KeywordType::other)
                {
                    int numKeys = keywordsTree.getNumChildren();
                    
                    if(numKeys == 0)
                    {
                        startIndex = 0;
                    }
                    else
                    {
                        startIndex = numKeys;
                    }
                }
            }
            
            for(int i = 0; i < changedWords.size(); i++)
            {
                ValueTree currentKey = keywordsTree.getChild(i + startIndex);
                
                if(currentKey.isValid() && currentKey.getProperty("Type") == typeToLookFor)
                {
                    if(currentKey.getProperty("Name") != changedWords[i])
                    {
                        currentKey.setProperty("Name", changedWords[i], nullptr);
                    }
                }
                else
                {
                    ValueTree newKey("Word");
                    newKey.setProperty("Name", changedWords[i], nullptr);
                    newKey.setProperty("Type", typeToLookFor, nullptr);
                    keywordsTree.addChild(newKey, i + startIndex, nullptr);
                }
            }
            
            if(endIndex != -1 && (endIndex - startIndex) + 1 > changedWords.size())
            {
                for(int indexToRemove = startIndex + changedWords.size(); indexToRemove <= endIndex; indexToRemove++)
                {
                    keywordsTree.removeChild(indexToRemove, nullptr);
                }
            }
            
            checkAndUpdateIfFXIsReady(fxTree);
        }
    });
    
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
        
        fileList.updateContent();
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
            
            else if(treeToSearch.getChild(i).getProperty("Type") == "Unspec")
            {
                return -1;
            }
        }
        return -1;
    }
    
    //Unspecified type
    for(int i = 0; i < treeToSearch.getNumChildren(); i++)
    {
        if(treeToSearch.getChild(i).getProperty("Type") == "Unspec")
        {
            return i;
        }
    }
    return -1;
}

int AddFilesComponent::getLastIndexOfKeywordType(const ValueTree& treeToSearch, AddFilesParameterEditor::KeywordType wordTypeToLookFor) const
{
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
            
        case AddFilesParameterEditor::KeywordType::other:
            wordToLookFor = "Unspec";
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

bool AddFilesComponent::checkIfFileAlreadyExistsInDatabase(const String& pathToLookFor) const
{
    ValueTree fxList = dataToAddTo.getChildWithName("FXList");
    
    if(std::any_of(fxList.begin(), fxList.end(), [&pathToLookFor](const ValueTree& fx)
    {
        if(fx.getType().toString() == "FX")
        {
            if(fx.getProperty("Path").toString() == pathToLookFor)
            {
                return true;
            }
        }
        return false;
    }))
    {
        return true;
    }
    
    ValueTree newFilesList = fxList.getChildWithName("NewFiles");
    
    return std::any_of(newFilesList.begin(), newFilesList.end(), [&pathToLookFor](const ValueTree& fx)
    {
        if(fx.getType().toString() == "FX")
        {
            if(fx.getProperty("Path").toString() == pathToLookFor)
            {
                return true;
            }
        }
        return false;
    });
}
