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
HierachicalListBrowser::HierachicalListBrowser()
{
    
}

HierachicalListBrowser::~HierachicalListBrowser()
{
}

void HierachicalListBrowser::paint (juce::Graphics& g)
{
    if(dataToDisplay.isValid())
    {
        drawChildren(g, dataToDisplay);
    }
}

void HierachicalListBrowser::resized()
{
    
}

void HierachicalListBrowser::setDataToDisplay(juce::ValueTree newData)
{
    dataToDisplay = newData;
    repaint();
}

int HierachicalListBrowser::drawChildren(juce::Graphics& g, juce::ValueTree treeToDraw, int startY, int indentationIndex)
{
    int yCoord = startY;
    int thisIndentation = indentationIndex * 10;
    
    std::for_each(treeToDraw.begin(), treeToDraw.end(), [this, &g, &indentationIndex, &yCoord, thisIndentation](juce::ValueTree tree)
    {
        if(tree.getType() == juce::Identifier("Catagory"))
        {
            tree.setProperty("YLocation", yCoord, nullptr);
            
            g.drawText(tree.getProperty("Name"), thisIndentation + 10, yCoord, getWidth() - thisIndentation, 20, juce::Justification::left);
            
            if(tree.getNumChildren() == 0)
            {
                g.drawLine(thisIndentation + 1, yCoord + 10, thisIndentation + 7, yCoord + 10, 1.5);
                yCoord+=20;
            }
            
            else if(!tree.getProperty("Opened"))
            {
                g.drawLine(thisIndentation + 1, yCoord + 6, thisIndentation + 7, yCoord + 10, 1.5);
                g.drawLine(thisIndentation + 1, yCoord + 14, thisIndentation + 7, yCoord + 10, 1.5);
                yCoord+=20;
            }
            
            else if(tree.getProperty("Opened"))
            {
                g.drawLine(thisIndentation + 1, yCoord + 6, thisIndentation + 5, yCoord + 14, 1.5);
                g.drawLine(thisIndentation + 9, yCoord + 6, thisIndentation + 5, yCoord + 14, 1.5);
                
                yCoord+=20;
                yCoord = drawChildren(g, tree, yCoord, indentationIndex + 1);
            }
        }
    });
    
    return yCoord;
}

void HierachicalListBrowser::mouseDown(const juce::MouseEvent &event)
{
    bool found = false;
    juce::ValueTree treeToSearch = dataToDisplay;
    
    //Finds the lowest position on the tree and rejects anything lower than this
    int maximumY = getBottomNode(treeToSearch).getProperty("YLocation");
    if(event.getMouseDownY() < maximumY + 20)
    {
        //Everytime a new child is explored the indentation index is put up one
        int indentationIndex = 0;
            
        for(int i = 0; i < treeToSearch.getNumChildren() && found == false; i++)
        {
            //Goes through the children until the one lower than the one being checked is invalid or lower than the click point
            juce::ValueTree childToCheck = treeToSearch.getChild(i + 1);
            
            int mouseY = event.getMouseDownY();
            
            if(float(childToCheck.getProperty("YLocation")) > mouseY || !childToCheck.isValid())
            {
                //if the child that has been identified as possibly being clicked has no children or is not open we know it is this that has been clicked
                if(treeToSearch.getChild(i).getNumChildren() == 0 || !treeToSearch.getChild(i).getProperty("Opened") || mouseY < float(treeToSearch.getChild(i).getProperty("YLocation")) + 20)
                {
                    //Checks its indentation
                    if(event.getMouseDownX() > indentationIndex * 10 && event.getMouseDownX() < (indentationIndex + 1) * 10)
                    {
                        DBG(juce::String(treeToSearch.getChild(i).getProperty("Name")));
                        found = true;
                    }
                }
                //If it is open we go into its chilren
                else
                {
                    treeToSearch = treeToSearch.getChild(i);
                    i = -1;
                    indentationIndex+=1;
                }
            }
        }
    }
}

juce::ValueTree HierachicalListBrowser::getBottomNode(juce::ValueTree inputTree)
{
    juce::ValueTree lastChild = inputTree.getChild(inputTree.getNumChildren() - 1);
    
    if(lastChild.getNumChildren() == 0)
    {
        return lastChild;
    }
    else
    {
        return getBottomNode(lastChild);
    }
}
