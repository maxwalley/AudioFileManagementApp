/*
  ==============================================================================

    HierachicalListBrowser.cpp
    Created: 16 Sep 2020 11:41:39am
    Author:  Max Walley

  ==============================================================================
*/

#include <JuceHeader.h>
#include "HierachicalListBrowser.h"

//==============================================================================

AddButton::AddButton() : juce::Button(juce::String())
{
    
}

AddButton::~AddButton()
{
    
}

void AddButton::paintButton (juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    //Button Fill
    g.setColour(findColour(juce::TextButton::buttonColourId));
    g.fillEllipse(1, 1, getWidth() - 2, getHeight() - 2);
    
    g.setColour(juce::Colours::black);
    g.drawEllipse(1, 1, getWidth() - 2, getHeight() - 2, 1.5);
    
    g.drawLine(getWidth() / 5, getHeight() / 2, (getWidth() / 5) * 4, getHeight() / 2, 1.5);
    g.drawLine(getWidth() / 2, getHeight() / 5, getWidth() / 2, (getHeight() / 5) * 4, 1.5);
}


HierachicalListBrowser::HierachicalListBrowser() : dataFormatted(false), folderName("Folder")
{
    addAndMakeVisible(addButton);
    addButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightgrey);
    addButton.addListener(this);
    
    dataToDisplay.addListener(this);
    originalData.addListener(this);
    
    setColour(int(ColourIds::textColourId), juce::Colours::black);
    setColour(int(ColourIds::symbolColourId), juce::Colours::black);
    setColour(int(ColourIds::highlightColourId), juce::Colours::black);
    setColour(int(ColourIds::backgroundColourId), juce::Colours::white);
    setColour(int(ColourIds::highlightTextColourId), juce::Colours::white);
}

HierachicalListBrowser::~HierachicalListBrowser()
{
}

void HierachicalListBrowser::paint (juce::Graphics& g)
{
    if(dataFormatted)
    {
        drawChildren(g, dataToDisplay);
    }
    
    g.setColour(juce::Colours::black);
    g.drawLine(0, getHeight() - 35, getWidth(), getHeight() - 35, 1.5);
    
    g.setFont(juce::Font(16));
    g.drawText("Add New " + folderName, getWidth() / 2, getHeight() - 35, getWidth() / 2 - 30, 35, juce::Justification::centred);
}

void HierachicalListBrowser::resized()
{
    if(dataFormatted)
    {
        drawLabels(dataToDisplay);
    }
    
    addButton.setBounds(getWidth() - 30, getHeight() - 30, 30, 30);
}

void HierachicalListBrowser::setDataToDisplay(juce::ValueTree newData)
{
    originalData = newData;
    dataToDisplay = newData.createCopy();
    
    refresh();
}

void HierachicalListBrowser::refresh()
{
    nameLabels.resize(getNumberOfViewableNodes(dataToDisplay));
    
    std::for_each(nameLabels.begin(), nameLabels.end(), [this](std::unique_ptr<juce::Label>& label)
    {
        if(label == nullptr)
        {
            label = std::make_unique<juce::Label>();
            label->setJustificationType(juce::Justification::left);
            label->addListener(this);
            label->addMouseListener(this, true);
            label->setComponentID("label");
            addAndMakeVisible(label.get());
        }
    });
    
    formatTree(dataToDisplay);
    dataFormatted = true;
    resized();
    repaint();
}

void HierachicalListBrowser::setFolderName(const juce::String& newName)
{
    folderName = newName;
}

juce::StringArray HierachicalListBrowser::getAllHighlightedItems() const
{
    return getHighlightedForTree(dataToDisplay);
}

void HierachicalListBrowser::addTypesToIgnore(const juce::StringArray& listOfTypes)
{
    typesToIgnore.addArray(listOfTypes);
    
    refresh();
}

void HierachicalListBrowser::drawChildren(juce::Graphics& g, juce::ValueTree treeToDraw)
{
    for(juce::ValueTree tree: treeToDraw)
    {
        if(typesToIgnore.contains(tree.getType()))
        {
            continue;
        }
        
        int yLocation = tree.getProperty("YLocation");
        int indentation = int(tree.getProperty("Indentation")) * 10;
        
        bool isOpen = tree.getProperty("Opened");
        int numChildren = tree.getNumChildren();
            
        g.setColour(findColour(int(HierachicalListBrowser::ColourIds::backgroundColourId)));
        g.fillRect(0, yLocation, getWidth(), 20);
        
        drawSymbol(g, yLocation, indentation, numChildren, isOpen);
        
        if(numChildren && isOpen)
        {
            drawChildren(g, tree);
        }
    };
}

void HierachicalListBrowser::drawLabels(juce::ValueTree treeToDraw)
{
    for(int i = 0; i < treeToDraw.getNumChildren(); i++)
    {
        juce::ValueTree currentTree = treeToDraw.getChild(i);
        
        if(typesToIgnore.contains(currentTree.getType()))
        {
            continue;
        }
        
        int yLoc = currentTree.getProperty("YLocation");
        int indent = int(currentTree.getProperty("Indentation")) * 10;
        
        nameLabels[yLoc / 20]->setBounds(indent + 10, yLoc, getWidth() - (indent + 10), 20);
        
        if(treeToDraw.getChild(i).getProperty("Highlight"))
        {
            nameLabels[yLoc / 20]->setColour(juce::Label::backgroundColourId, findColour(int(ColourIds::highlightColourId)));
            nameLabels[yLoc / 20]->setColour(juce::Label::textColourId, findColour(int(ColourIds::highlightTextColourId)));
        }
        else
        {
            nameLabels[yLoc / 20]->setColour(juce::Label::backgroundColourId, findColour(int(ColourIds::backgroundColourId)));
            nameLabels[yLoc / 20]->setColour(juce::Label::textColourId, findColour(int(ColourIds::textColourId)));
        }
        
        nameLabels[yLoc / 20]->setEditable(false, treeToDraw.getChild(i).getProperty("Renameable"));
        
        if(currentTree.getNumChildren() > 0 && currentTree.getProperty("Opened"))
        {
            drawLabels(currentTree);
        }
    }
}

int HierachicalListBrowser::formatTree(juce::ValueTree inputTree, int startY, int startIndentationIndex)
{
    for (juce::ValueTree tree : inputTree)
    {
        if(typesToIgnore.contains(tree.getType()))
        {
            continue;
        }
        
        if(!tree.hasProperty("Opened"))
        {
            tree.setProperty("Opened", false, nullptr);
        }
        
        if(!tree.hasProperty("Highlight"))
        {
            tree.setProperty("Highlight", false, nullptr);
        }
        
        if(!tree.hasProperty("Renameable"))
        {
            tree.setProperty("Renameable", false, nullptr);
        }
        
        tree.setProperty("YLocation", startY, nullptr);
        tree.setProperty("Indentation", startIndentationIndex, nullptr);
        
        nameLabels[startY / 20]->setText(tree.getProperty("Name"), juce::dontSendNotification);
        
        startY+=20;
        
        if(tree.getNumChildren() > 0 && tree.getProperty("Opened"))
        {
            startY = formatTree(tree, startY, startIndentationIndex + 1);
        }
    };
    return startY;
}

void HierachicalListBrowser::labelTextChanged(juce::Label* label)
{
    juce::ValueTree labelTree = getNodeAtYVal(label->getY(), dataToDisplay);
    
    labelTree.setProperty("Name", label->getText(), nullptr);
}

void HierachicalListBrowser::drawSymbol(juce::Graphics& g, int yStart, int indentation, bool hasChildren, bool isOpen)
{
    g.setColour(findColour(int(ColourIds::symbolColourId)));
        
    if(!hasChildren)
    {
        g.drawLine(indentation + 1, yStart + 10, indentation + 7, yStart + 10, 1.5);
    }
    
    else if(!isOpen)
    {
        g.drawLine(indentation + 1, yStart + 6, indentation + 7, yStart + 10, 1.5);
        g.drawLine(indentation + 1, yStart + 14, indentation + 7, yStart + 10, 1.5);
    }
    
    else if(isOpen)
    {
        g.drawLine(indentation + 1, yStart + 6, indentation + 5, yStart + 14, 1.5);
        g.drawLine(indentation + 9, yStart + 6, indentation + 5, yStart + 14, 1.5);
    }
}

void HierachicalListBrowser::mouseDown(const juce::MouseEvent &event)
{
    if(event.originalComponent->getComponentID() == "label")
    {
        juce::ValueTree labelTree = getNodeAtYVal(event.originalComponent->getY(), dataToDisplay);
        
        labelTree.setProperty("Highlight", !labelTree.getProperty("Highlight"), nullptr);
        return;
    }
    
    //Finds the lowest position on the tree and rejects anything lower than this
    int maximumY = getBottomNode(dataToDisplay).getProperty("YLocation");
    if(event.getMouseDownY() < maximumY + 20)
    {
        juce::ValueTree clickedTree = getNodeAtYVal(event.getMouseDownY(), dataToDisplay);
        
        int indentation = int(clickedTree.getProperty("Indentation")) * 10;
        
        if(event.getMouseDownX() > indentation && event.getMouseDownX() < indentation + 10)
        {
            //A tree with no children cannot be opened
            if(clickedTree.getNumChildren() > 0)
            {
                clickedTree.setProperty("Opened", !clickedTree.getProperty("Opened"), nullptr);
                
                if(!clickedTree.getProperty("Opened"))
                {
                    setAllPropertiesInATree(clickedTree, "Highlight", false, false, true);
                }
            }
        }
    }
}

void HierachicalListBrowser::buttonClicked(juce::Button* button)
{
    if(button == &addButton)
    {
        addChildrenToHighlights(dataToDisplay);
    }
}

void HierachicalListBrowser::valueTreeChildAdded(juce::ValueTree &parentTree, juce::ValueTree &childWhichHasBeenAdded)
{
    //This property stops it from recursively adding
    if(childWhichHasBeenAdded.hasProperty("Stop"))
    {
        childWhichHasBeenAdded.removeProperty("Stop", nullptr);
        return;
    }
    
    int newChildIndex = parentTree.indexOf(childWhichHasBeenAdded);
    juce::ValueTree newChild = childWhichHasBeenAdded.createCopy();
    
    newChild.setProperty("Stop", true, nullptr);
    
    parentTree.removeChild(childWhichHasBeenAdded, nullptr);
    
    juce::ValueTree parentInOtherTree;
    
    juce::ValueTree rootTree = parentTree.getRoot();
    
    if(rootTree == originalData.getParent())
    {
        parentInOtherTree = findTreeInOtherTree(parentTree, dataToDisplay);
    }
    else if(rootTree == dataToDisplay)
    {
        parentInOtherTree = findTreeInOtherTree(parentTree, originalData);
    }
    
    parentTree.addChild(newChild.createCopy(), newChildIndex, nullptr);
    parentInOtherTree.addChild(newChild.createCopy(), newChildIndex, nullptr);
    
    refresh();
}

void HierachicalListBrowser::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    if(property == juce::Identifier("Name"))
    {
        juce::ValueTree treeThatNeedsProperty;
        
        juce::ValueTree rootTree = treeWhosePropertyHasChanged.getRoot();
        
        if(rootTree == originalData.getParent())
        {
            treeThatNeedsProperty = findTreeInOtherTree(treeWhosePropertyHasChanged, dataToDisplay);
        }
        else if(rootTree == dataToDisplay)
        {
            treeThatNeedsProperty = findTreeInOtherTree(treeWhosePropertyHasChanged, originalData);
        }
        
        treeThatNeedsProperty.setPropertyExcludingListener(this, property, treeWhosePropertyHasChanged.getProperty(property), nullptr);
    }
    
    refresh();
}

void HierachicalListBrowser::addChildrenToHighlights(juce::ValueTree tree)
{
    for (juce::ValueTree treeToSearch : tree)
    {
        if(typesToIgnore.contains(treeToSearch.getType()))
        {
            continue;
        }
        
        if(treeToSearch.getNumChildren() > 0 && treeToSearch.getProperty("Opened"))
        {
            addChildrenToHighlights(treeToSearch);
        }
        
        if(treeToSearch.getProperty("Highlight"))
        {
            juce::ValueTree newTree(folderName);
            newTree.setProperty("Renameable", true, nullptr);
            newTree.setProperty("Name", "New " + folderName, nullptr);
            
            treeToSearch.addChild(newTree, -1, nullptr);
        }
    };
}

juce::ValueTree HierachicalListBrowser::getBottomNode(juce::ValueTree inputTree) const
{
    juce::ValueTree lastChild;
    
    for(int i = inputTree.getNumChildren() - 1; i >= 0 && !lastChild.isValid(); i--)
    {
        juce::ValueTree testChild = inputTree.getChild(i);
        
        if(!typesToIgnore.contains(testChild.getType()))
        {
            lastChild = testChild;
        }
    }
    
    //If all in this one were invalid
    if(!lastChild.isValid())
    {
        return inputTree;
    }
        
    if(lastChild.getNumChildren() == 0 || !lastChild.getProperty("Opened"))
    {
        return lastChild;
    }
    
    //if the last valid one is open
    return getBottomNode(lastChild);
}

const int HierachicalListBrowser::getNumberOfViewableNodes(juce::ValueTree inputTree) const
{
    int count = 0;
    
    for (juce::ValueTree tree : inputTree)
    {
        if(typesToIgnore.contains(tree.getType()))
        {
            continue;
        }
        
        count++;
        
        if(tree.getProperty("Opened"))
        {
            count += getNumberOfViewableNodes(tree);
        }
    };
    
    return count;
}

juce::ValueTree HierachicalListBrowser::getNodeAtYVal(int yVal, juce::ValueTree treeToSearch) const
{
    for(int i = 0; i < treeToSearch.getNumChildren(); i++)
    {
        //If the one being checked is not valid
        if(typesToIgnore.contains(treeToSearch.getChild(i).getType()))
        {
            continue;
        }
        
        juce::ValueTree testTree;
        
        //Finds the next valid one
        for(int j = i + 1; j < treeToSearch.getNumChildren() && !testTree.isValid(); j++)
        {
            if(!typesToIgnore.contains(treeToSearch.getChild(j).getType()))
            {
                testTree = treeToSearch.getChild(j);
            }
        }
        
        int thisNodeYLoc = testTree.getProperty("YLocation");
        
        if(thisNodeYLoc > yVal || !testTree.isValid())
        {
            juce::ValueTree evalTree = treeToSearch.getChild(i);
            
            if(evalTree.getNumChildren() == 0 || !evalTree.getProperty("Opened") || yVal < int(evalTree.getProperty("YLocation")) + 20)
            {
                return evalTree;
            }
            
            return getNodeAtYVal(yVal, evalTree);
        }
    }
    
    return treeToSearch;
}

juce::StringArray HierachicalListBrowser::getHighlightedForTree(juce::ValueTree treeToSearch) const
{
    juce::StringArray arrayToReturn;
    
    for (juce::ValueTree tree : treeToSearch)
    {
        if(tree.getProperty("Highlight"))
        {
            arrayToReturn.add(tree.getProperty("Name"));
        }
        
        if(tree.getNumChildren() > 0 && tree.getProperty("Opened"))
        {
            arrayToReturn.addArray(getHighlightedForTree(tree));
        }
    };
    
    return arrayToReturn;
}

void HierachicalListBrowser::setAllPropertiesInATree(juce::ValueTree inputTree, const juce::Identifier& property, const juce::var& val, bool affectTopTree, bool recursive)
{
    if(affectTopTree)
    {
        inputTree.setProperty(property, val, nullptr);
    }
    
    for (juce::ValueTree tree : inputTree)
    {
        if(typesToIgnore.contains(tree.getType()))
        {
            continue;
        }
        
        tree.setProperty(property, val, nullptr);
        
        if(recursive)
        {
            if(tree.getProperty("Opened"))
            {
                setAllPropertiesInATree(tree, property, val, false, true);
            }
        }
    }
}

bool HierachicalListBrowser::compareChildren(const juce::ValueTree& first, const juce::ValueTree& second) const
{
    if(first.getNumChildren() != second.getNumChildren())
    {
        return false;
    }
    
    for(int i = 0; i < first.getNumChildren(); i++)
    {
        if(first.getChild(i).getProperty("Name") != second.getChild(i).getProperty("Name"))
        {
            return false;
        }
        
        if(first.getChild(i).getNumChildren() > 0)
        {
            if(!compareChildren(first.getChild(i), second.getChild(i)))
            {
                return false;
            }
        }
    }
    return true;
}

juce::ValueTree HierachicalListBrowser::findTreeInOtherTree(const juce::ValueTree& treeToFind, const juce::ValueTree& treeToSearch) const
{
    for(int i = 0; i < treeToSearch.getNumChildren(); i++)
    {
        if(compareChildren(treeToSearch.getChild(i), treeToFind))
        {
            return treeToSearch.getChild(i);
        }
        
        if(treeToSearch.getChild(i).getNumChildren() > 0)
        {
            juce::ValueTree subSearchRes = findTreeInOtherTree(treeToFind, treeToSearch.getChild(i));
            
            if(subSearchRes.isValid())
            {
                return subSearchRes;
            }
        }
    }
    
    return juce::ValueTree();
}
