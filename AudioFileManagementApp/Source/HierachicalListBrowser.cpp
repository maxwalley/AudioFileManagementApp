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
        }
        addAndMakeVisible(label.get());
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

void HierachicalListBrowser::drawChildren(juce::Graphics& g, juce::ValueTree treeToDraw)
{
    std::for_each(treeToDraw.begin(), treeToDraw.end(), [this, &g](juce::ValueTree tree)
    {
        int yLocation = tree.getProperty("YLocation");
        int indentation = int(tree.getProperty("Indentation")) * 10;
            
        g.setColour(juce::Colours::black);
            
        if(tree.getNumChildren() == 0)
        {
            g.drawLine(indentation + 1, yLocation + 10, indentation + 7, yLocation + 10, 1.5);
        }
        
        else if(!tree.getProperty("Opened"))
        {
            g.drawLine(indentation + 1, yLocation + 6, indentation + 7, yLocation + 10, 1.5);
            g.drawLine(indentation + 1, yLocation + 14, indentation + 7, yLocation + 10, 1.5);
        }
        
        else if(tree.getProperty("Opened"))
        {
            g.drawLine(indentation + 1, yLocation + 6, indentation + 5, yLocation + 14, 1.5);
            g.drawLine(indentation + 9, yLocation + 6, indentation + 5, yLocation + 14, 1.5);
            
            drawChildren(g, tree);
        }
    });
}

void HierachicalListBrowser::drawLabels(juce::ValueTree treeToDraw)
{
    for(int i = 0; i < treeToDraw.getNumChildren(); i++)
    {
        juce::ValueTree currentTree = treeToDraw.getChild(i);
        int yLoc = currentTree.getProperty("YLocation");
        int indent = int(currentTree.getProperty("Indentation")) * 10;
        
        nameLabels[yLoc / 20]->setBounds(indent + 10, yLoc, getWidth() - (indent + 10), 20);
        
        if(treeToDraw.getChild(i).getProperty("Highlight"))
        {
            nameLabels[yLoc / 20]->setColour(juce::Label::backgroundColourId, juce::Colours::black);
            nameLabels[yLoc / 20]->setColour(juce::Label::textColourId, juce::Colours::white);
        }
        else
        {
            nameLabels[yLoc / 20]->setColour(juce::Label::backgroundColourId, juce::Colours::silver);
            nameLabels[yLoc / 20]->setColour(juce::Label::textColourId, juce::Colours::black);
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
    std::for_each(inputTree.begin(), inputTree.end(), [this, &startY, &startIndentationIndex](juce::ValueTree tree)
    {
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
    });
    return startY;
}

void HierachicalListBrowser::labelTextChanged(juce::Label* label)
{
    juce::ValueTree labelTree = getNodeAtYVal(label->getY(), dataToDisplay);
    
    labelTree.setProperty("Name", label->getText(), nullptr);
}

void HierachicalListBrowser::mouseDown(const juce::MouseEvent &event)
{
    if(event.originalComponent->getComponentID() == "label")
    {
        juce::ValueTree labelTree = getNodeAtYVal(event.originalComponent->getY(), dataToDisplay);
        
        labelTree.setProperty("Highlight", !labelTree.getProperty("Highlight"), nullptr);
        refresh();
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
                
                refresh();
            }
        }
    }
}

void HierachicalListBrowser::buttonClicked(juce::Button* button)
{
    if(button == &addButton)
    {
        addChildrenToHighlights(dataToDisplay);
        refresh();
    }
}

void HierachicalListBrowser::addChildrenToHighlights(juce::ValueTree tree)
{
    std::for_each(tree.begin(), tree.end(), [this](juce::ValueTree treeToSearch)
    {
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
    });
}

juce::ValueTree HierachicalListBrowser::getBottomNode(juce::ValueTree inputTree)
{
    juce::ValueTree lastChild = inputTree.getChild(inputTree.getNumChildren() - 1);
    
    if(lastChild.getNumChildren() == 0 || !lastChild.getProperty("Opened"))
    {
        return lastChild;
    }
    else
    {
        return getBottomNode(lastChild);
    }
}

const int HierachicalListBrowser::getNumberOfViewableNodes(juce::ValueTree inputTree) const
{
    int count = 0;
    
    std::for_each(inputTree.begin(), inputTree.end(), [this, &count](juce::ValueTree tree)
    {
        count++;
        
        if(tree.getProperty("Opened"))
        {
            count += getNumberOfViewableNodes(tree);
        }
    });
    
    return count;
}

juce::ValueTree HierachicalListBrowser::getNodeAtYVal(int yVal, juce::ValueTree treeToSearch) const
{
    for(int i = 0; i < treeToSearch.getNumChildren(); i++)
    {
        juce::ValueTree testTree = treeToSearch.getChild(i + 1);
        
        if(int(testTree.getProperty("YLocation")) > yVal || !testTree.isValid())
        {
            juce::ValueTree evalTree = treeToSearch.getChild(i);
            
            if(evalTree.getNumChildren() == 0 || !evalTree.getProperty("Opened") || yVal < int(evalTree.getProperty("YLocation")) + 20)
            {
                return evalTree;
            }
            
            return getNodeAtYVal(yVal, evalTree);
        }
    }
}

juce::StringArray HierachicalListBrowser::getHighlightedForTree(juce::ValueTree treeToSearch) const
{
    juce::StringArray arrayToReturn;
    
    std::for_each(treeToSearch.begin(), treeToSearch.end(), [this, &arrayToReturn](juce::ValueTree tree)
    {
        if(tree.getProperty("Highlight"))
        {
            arrayToReturn.add(tree.getProperty("Name"));
        }
        
        if(tree.getNumChildren() > 0 && tree.getProperty("Opened"))
        {
            arrayToReturn.addArray(getHighlightedForTree(tree));
        }
    });
    
    return arrayToReturn;
}
