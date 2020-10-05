/*
  ==============================================================================

    AddFilesParameterEditor.cpp
    Created: 16 Sep 2020 10:53:00am
    Author:  Max Walley

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AddFilesParameterEditor.h"

ValueTreeItem::ValueTreeItem(ValueTree treeToDisplay) : tree(treeToDisplay)
{
    refreshChildren();
    
    tree.addListener(this);
}

ValueTreeItem::~ValueTreeItem()
{
}

bool ValueTreeItem::mightContainSubItems()
{
    return tree.getNumChildren();
}

String ValueTreeItem::getUniqueName() const
{
    return tree.getProperty("Name").toString();
}

Component* ValueTreeItem::createItemComponent()
{
    Label* newLabel = new Label();
    
    newLabel->setText(tree.getProperty("Name"), dontSendNotification);
    newLabel->setEditable(false, true);
    newLabel->addMouseListener(this, false);
    newLabel->addListener(this);
    
    return newLabel;
}

void ValueTreeItem::paintItem(Graphics& g, int width, int height)
{
    if(!isSelected())
    {
        g.setColour(Colours::silver);
    }
    else
    {
        g.setColour(Colours::black);
    }
    
    g.fillAll();
}

void ValueTreeItem::itemSelectionChanged(bool isSelected)
{
    repaintItem();
}

void ValueTreeItem::mouseDown(const MouseEvent& event)
{
    setSelected(!isSelected(), false);
}

void ValueTreeItem::labelTextChanged(Label* label)
{
    tree.setProperty("Name", label->getText(), nullptr);
}

void ValueTreeItem::valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded)
{
    if(parentTree == tree)
    {
        addSubItem(new ValueTreeItem(childWhichHasBeenAdded));
    }
}

void ValueTreeItem::valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int index)
{
    if(parentTree == tree)
    {
        removeSubItem(index);
    }
}

void ValueTreeItem::valueTreeChildOrderChanged(ValueTree& parentTree, int oldIndex, int newIndex)
{
    if(parentTree == tree)
    {
        refreshChildren();
    }
}

ValueTree ValueTreeItem::getShownTree() const
{
    return tree;
}

void ValueTreeItem::refreshChildren()
{
    if(getNumSubItems() != tree.getNumChildren())
    {
        clearSubItems();
        
        std::for_each(tree.begin(), tree.end(), [this](ValueTree curTree)
        {
            addSubItem(new ValueTreeItem(curTree));
        });
    }
}

//==============================================================================
AddFilesParameterEditor::AddFilesParameterEditor(juce::ValueTree currentData) : dataToAddTo(currentData), newCatButton("New Catagory"), removeCatButton("Remove Catagory"), lastDescripEditorText("")
{
    addAndMakeVisible(titleLabel);
    titleLabel.setText("Parameters", dontSendNotification);
    titleLabel.setFont(Font(20));
    titleLabel.setJustificationType(Justification::centred);
    
    newVersionToggle.addListener(this);
    addAndMakeVisible(newVersionToggle);
    newVersionLabel.setText("Files are new versions of exisiting files", dontSendNotification);
    newVersionLabel.attachToComponent(&newVersionToggle, true);
    
    catagoryViewer.setRootItem(new ValueTreeItem(dataToAddTo.getChildWithName("Catagories")));
    catagoryViewer.setRootItemVisible(false);
    addAndMakeVisible(catagoryViewer);
    
    addAndMakeVisible(newCatButton);
    newCatButton.addListener(this);
    newCatButton.setTooltip("Add a new catagory to each of the highlighted items");
    
    addAndMakeVisible(removeCatButton);
    removeCatButton.addListener(this);
    removeCatButton.setTooltip("Remove all selected catagories. Warning: this will remove this catagory from all fx using it");
    
    addAndMakeVisible(nounLabel);
    nounLabel.setColour(Label::ColourIds::outlineColourId, Colours::black);
    nounLabel.setEditable(true);
    nounLabel.addListener(this);
    
    addAndMakeVisible(verbLabel);
    verbLabel.setColour(Label::ColourIds::outlineColourId, Colours::black);
    verbLabel.setEditable(true);
    verbLabel.addListener(this);
    
    addAndMakeVisible(descripEditor);
    descripEditor.setMultiLine(true);
    descripEditor.setColour(TextEditor::ColourIds::backgroundColourId, Colours::silver);
    descripEditor.setColour(TextEditor::outlineColourId, Colours::black);
    descripEditor.setTooltip("These words should describe the effect and be seperated with a ','");
    descripEditor.addListener(this);
}

AddFilesParameterEditor::~AddFilesParameterEditor()
{
    catagoryViewer.deleteRootItem();
}

void AddFilesParameterEditor::paint (juce::Graphics& g)
{
    if(!dataToShow.isValid())
    {
        return;
    }
    
    if(!newVersionToggle.getToggleState())
    {
        g.drawText("Choose Catagories", 0, 80, getWidth(), 10, Justification::left);
        g.drawText("Noun", 10, 290, 120, 15, Justification::centred);
        g.drawText("Verb", getWidth() - 130, 290, 120, 15, Justification::centred);
        g.drawText("Descriptive Words", 10, 335, getWidth() - 20, 15, Justification::left);
    }
}

void AddFilesParameterEditor::resized()
{
    titleLabel.setBounds(getWidth() / 3, 10, getWidth() / 3, 40);
    
    if(!dataToShow.isValid())
    {
        newVersionToggle.setVisible(false);
        catagoryViewer.setVisible(false);
        newCatButton.setVisible(false);
        removeCatButton.setVisible(false);
        nounLabel.setVisible(false);
        verbLabel.setVisible(false);
        descripEditor.setVisible(false);
        return;
    }
    
    catagoryViewer.setVisible(true);
    newVersionToggle.setVisible(true);
    newCatButton.setVisible(true);
    removeCatButton.setVisible(true);
    nounLabel.setVisible(true);
    verbLabel.setVisible(true);
    descripEditor.setVisible(true);
    
    newVersionToggle.setBounds(getWidth() / 6 * 5, 50, 22, 20);
    
    if(!newVersionToggle.getToggleState())
    {
        catagoryViewer.setBounds(0, 95, getWidth(), 155);
        newCatButton.setBounds(10, 260, 120, 20);
        removeCatButton.setBounds(getWidth() - 130, 260, 120, 20);
        nounLabel.setBounds(10, 305, 120, 20);
        verbLabel.setBounds(getWidth() - 130, 305, 120, 20);
        descripEditor.setBounds(10, 350, getWidth() - 20, 40);
    }
    else
    {
        catagoryViewer.setVisible(false);
    }
}

void AddFilesParameterEditor::setDataToShow(ValueTree newData)
{
    dataToShow = newData;
    resized();
    repaint();
    
    //If something has been sent
    if(dataToShow.isValid())
    {
        ValueTree keywordsChild = dataToShow.getChildWithName("Keywords");
        
        if(keywordsChild.isValid())
        {
            String nounKeywords;
            String verbKeywords;
            String descripKeywords;
        
            std::for_each(keywordsChild.begin(), keywordsChild.end(), [&nounKeywords, &verbKeywords, &descripKeywords](const ValueTree& tree)
            {
                String curKey = tree.getProperty("Name");
                
                var type = tree.getProperty("Type");
                
                if(type.isVoid())
                {
                    if(!descripKeywords.isEmpty())
                    {
                        descripKeywords += ", " + curKey;
                    }
                    else
                    {
                        descripKeywords = curKey;
                    }
                }
                
                else if (type == "Noun")
                {
                    if(!nounKeywords.isEmpty())
                    {
                        nounKeywords += ", " + curKey;
                    }
                    
                    else
                    {
                        nounKeywords = curKey;
                    }
                }
                
                else if (type == "Verb")
                {
                    if(!verbKeywords.isEmpty())
                    {
                        verbKeywords += ", " + curKey;
                    }
                    
                    else
                    {
                        verbKeywords = curKey;
                    }
                }
            });
        
            nounLabel.setText(nounKeywords, dontSendNotification);
            verbLabel.setText(verbKeywords, dontSendNotification);
            descripEditor.setText(descripKeywords, dontSendNotification);
            
            lastDescripEditorText = descripKeywords;
        }
        
        ValueTree catChild = dataToShow.getChildWithName("Catagories");
        
        if(catChild.isValid())
        {
            //Extracts the category names and highlights them in the viewer
            
            std::for_each(catChild.begin(), catChild.end(), [this](const ValueTree& tree)
            {
                lookAndHighlightCatagory(tree.getProperty("Name"), catagoryViewer.getRootItem());
            });
        }
    }
    
    else
    {
        descripEditor.setText("");
        catagoryViewer.clearSelectedItems();
    }
}

void AddFilesParameterEditor::addListener(Listener* newListener)
{
    listeners.push_back(newListener);
}

void AddFilesParameterEditor::removeListener(Listener* listenerToRemove)
{
    std::remove(listeners.begin(), listeners.end(), listenerToRemove);
}

StringArray AddFilesParameterEditor::getTextFromComponent(KeywordType typeOfDataToFetch) const
{
    StringArray keywordsToReturn;
    
    switch (typeOfDataToFetch)
    {
        case KeywordType::noun:
            keywordsToReturn = seperateTextByCommaIntoArray(nounLabel.getText());
            break;
            
        case KeywordType::verb:
            keywordsToReturn = seperateTextByCommaIntoArray(verbLabel.getText());
            break;
            
        case KeywordType::other:
            keywordsToReturn = seperateTextByCommaIntoArray(descripEditor.getText());
            break;
    }
    
    return keywordsToReturn;
}

void AddFilesParameterEditor::mouseDown(const MouseEvent& event)
{
    if(event.originalComponent == this)
    {
        Component::unfocusAllComponents();
    }
}

void AddFilesParameterEditor::buttonClicked(Button* button)
{
    if(button == &newVersionToggle)
    {
        resized();
    }
    
    else if(button == &newCatButton)
    {
        int numSelected = catagoryViewer.getNumSelectedItems();
        
        if(numSelected == 0)
        {
            ValueTreeItem* rootItem = dynamic_cast<ValueTreeItem*>(catagoryViewer.getRootItem());
            
            if(rootItem != nullptr)
            {
                ValueTree newChild("Catagory");
                newChild.setProperty("Name", "New Catagory", nullptr);
                rootItem->getShownTree().appendChild(newChild, nullptr);
            }
            
            return;
        }
        
        for(int i = 0; i < numSelected; i++)
        {
            ValueTreeItem* selectedItem = dynamic_cast<ValueTreeItem*>(catagoryViewer.getSelectedItem(i));
            
            if(selectedItem == nullptr)
            {
                continue;
            }
            
            ValueTree newChild("Catagory");
            newChild.setProperty("Name", "New Catagory", nullptr);
            
            selectedItem->getShownTree().appendChild(newChild, nullptr);
        }
    }
    
    else if(button == &removeCatButton)
    {
        deleteSelectedItems();
    }
}

bool AddFilesParameterEditor::keyPressed(const KeyPress& key)
{
    if(key == KeyPress::deleteKey || key == KeyPress::backspaceKey)
    {
        deleteSelectedItems();
    }
    
    return true;
}

void AddFilesParameterEditor::labelTextChanged(Label* label)
{
    //ValueTree keywordsTree = dataToShow.getChildWithName("Keywords");
    
    KeywordType typeThatHasBeenChanged;
    
    if(label == &nounLabel)
    {
        typeThatHasBeenChanged = KeywordType::noun;
    }
    else if(label == &verbLabel)
    {
        typeThatHasBeenChanged = KeywordType::verb;
    }
    
    /*if(label == &nounLabel)
    {
        typeThatHasBeenChanged = KeywordType::noun;
        
        StringArray nouns = seperateTextByCommaIntoArray(nounLabel.getText());
        
        int lastNounIndex = getIndexOfLastKeywordType(KeywordType::noun);
        
        for(int i = 0; i < nouns.size(); i++)
        {
            if (i > lastNounIndex || lastNounIndex == -1)
            {
                ValueTree newKeyword("Word");
                newKeyword.setProperty("Name", nouns[i], nullptr);
                newKeyword.setProperty("Type", "Noun", nullptr);
                keywordsTree.addChild(newKeyword, i, nullptr);
            }
            
            else
            {
                if(keywordsTree.getChild(i).getProperty("Name") != nouns[i])
                {
                    keywordsTree.getChild(i).setProperty("Name", nouns[i], nullptr);
                }
            }
        }
        
        for(int i = nouns.size(); i < lastNounIndex; i++)
        {
            keywordsTree.removeChild(i, nullptr);
        }
    }
    
    else if(label == &verbLabel)
    {
        typeThatHasBeenChanged = KeywordType::verb;
        
        StringArray verbs = seperateTextByCommaIntoArray(verbLabel.getText());
        
        int lastVerbIndex = getIndexOfLastKeywordType(KeywordType::verb);
        int firstVerbIndex = getIndexOfFirstKeywordType(KeywordType::verb);
        
        for(int i = 0; i < verbs.size(); i++)
        {
            int j = i + firstVerbIndex;
            
            if (j > lastVerbIndex || lastVerbIndex == -1)
            {
                ValueTree newKeyword("Word");
                newKeyword.setProperty("Name", verbs[i], nullptr);
                newKeyword.setProperty("Type", "Verb", nullptr);
                keywordsTree.addChild(newKeyword, j, nullptr);
            }
            
            else
            {
                if(keywordsTree.getChild(j).getProperty("Name") != verbs[i])
                {
                    keywordsTree.getChild(j).setProperty("Name", verbs[i], nullptr);
                }
            }
        }
        
        for(int i = verbs.size() + firstVerbIndex; i < lastVerbIndex; i++)
        {
            keywordsTree.removeChild(i, nullptr);
        }
    }*/
    
    std::for_each(listeners.begin(), listeners.end(), [&typeThatHasBeenChanged](Listener* lis)
    {
        lis->dataChanged(typeThatHasBeenChanged);
    });
}

void AddFilesParameterEditor::textEditorFocusLost(TextEditor& editor)
{
    //If the text has changed
    if(editor.getText() != lastDescripEditorText)
    {
        /*
        ValueTree keywordsTree = dataToShow.getChildWithName("Keywords");
        
        lastDescripEditorText = editor.getText();
        
        StringArray words = seperateTextByCommaIntoArray(editor.getText());
        int firstKeywordIndex = getIndexOfFirstKeywordType(KeywordType::other);
        
        for(int i = 0; i < words.size(); i++)
        {
            int indexToLook = i + firstKeywordIndex;
            
            if(indexToLook > keywordsTree.getNumChildren() || firstKeywordIndex == -1)
            {
                ValueTree newKeyword("Word");
                newKeyword.setProperty("Name", words[i], nullptr);
                keywordsTree.appendChild(newKeyword, nullptr);
            }
            
            else
            {
                if(keywordsTree.getChild(indexToLook).getProperty("Name") != words[i])
                {
                    keywordsTree.getChild(i).setProperty("Name", words[i], nullptr);
                }
            }
        }
        
        for(int i = 0 + firstKeywordIndex; i < keywordsTree.getNumChildren(); i++)
        {
            keywordsTree.removeChild(i, nullptr);
        }*/
        
        std::for_each(listeners.begin(), listeners.end(), [](Listener* lis)
        {
            lis->dataChanged(KeywordType::other);
        });
    }
}

void AddFilesParameterEditor::deleteSelectedItems()
{
    int numSelected = catagoryViewer.getNumSelectedItems();
    
    if(numSelected == 0)
    {
        return;
    }
    
    if(AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, "Are you sure you want to delete this catagory?", "Deleting these catagories will delete them from any associated fx. Are you sure you want to continue?", "Continue", "Cancel", this))
    {
    /*
    for(int i = 0; i < numSelected; i++)
    {
        ValueTreeItem* selectedItem = dynamic_cast<ValueTreeItem*>(catagoryViewer.getSelectedItem(i));
        
        if(selectedItem == nullptr)
        {
            continue;
        }
        
        ValueTree selectedTree = selectedItem->getShownTree();
        
        selectedTree.getParent().removeChild(selectedTree, nullptr);
        
    }*/
        DBG("This is not yet implemented - catagories will need to be removed from fx");
    }
}

void AddFilesParameterEditor::lookAndHighlightCatagory(const String& catagory, TreeViewItem* treeToSearch)
{
    for(int i = 0; i < treeToSearch->getNumSubItems(); i++)
    {
        ValueTreeItem* tree = dynamic_cast<ValueTreeItem*>(treeToSearch->getSubItem(i));
        
        if(tree == nullptr)
        {
            continue;
        }
        
        if(tree->getShownTree().getProperty("Name").toString() == catagory)
        {
            tree->getParentItem()->setOpen(true);
            tree->setSelected(true, false);
            return;
        }
        
        if(tree->getNumSubItems() > 0)
        {
            lookAndHighlightCatagory(catagory, tree);
        }
    }
}

StringArray AddFilesParameterEditor::seperateTextByCommaIntoArray(const String& textToSeperate) const
{
    int commaIndex = textToSeperate.indexOfIgnoreCase(",");
    int lastComma = 0;
    
    StringArray strings;
    
    while(commaIndex != -1)
    {
        String subString = textToSeperate.substring(lastComma, commaIndex);
        
        if(lastComma != 0)
        {
            subString = subString.trimCharactersAtStart(", ");
        }
        
        strings.add(subString);
        
        lastComma = commaIndex;
        commaIndex = textToSeperate.indexOfIgnoreCase(++commaIndex, ",");
    }
    
    return strings;
}
