/*
  ==============================================================================

    AddFilesParameterEditor.cpp
    Created: 16 Sep 2020 10:53:00am
    Author:  Max Walley

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AddFilesParameterEditor.h"

//==============================================================================
AddFilesParameterEditor::AddFilesParameterEditor(juce::ValueTree currentData) : dataToAddTo(currentData)
{
    addAndMakeVisible(titleLabel);
    titleLabel.setText("Parameters", dontSendNotification);
    titleLabel.setFont(Font(20));
    titleLabel.setJustificationType(Justification::centred);
    
    newVersionToggle.addListener(this);
    addAndMakeVisible(newVersionToggle);
    newVersionLabel.setText("Files are new versions of exisiting files", dontSendNotification);
    newVersionLabel.attachToComponent(&newVersionToggle, true);
    
    /*ValueTree full("TestData");
    full.setProperty("Opened", false, nullptr);
    full.setProperty("Highlight", false, nullptr);
    
    ValueTree child("Catagory");
    child.setProperty("Name", "Child 1", nullptr);
    child.setProperty("Opened", true, nullptr);
    child.setProperty("Highlight", false, nullptr);
    
    ValueTree grandchild("Catagory");
    grandchild.setProperty("Name", "Grandchild 1", nullptr);
    grandchild.setProperty("Opened", false, nullptr);
    grandchild.setProperty("Highlight", false, nullptr);
    
    child.addChild(grandchild, -1, nullptr);
    
    full.addChild(child, -1, nullptr);
    
    ValueTree child2("Catagory");
    child2.setProperty("Name", "Child 2", nullptr);
    child2.setProperty("Opened", false, nullptr);
    child2.setProperty("Highlight", false, nullptr);
    
    full.addChild(child2, -1, nullptr);
    
    ValueTree child3("Catagory");
    child3.setProperty("Name", "Child 3", nullptr);
    child3.setProperty("Opened", true, nullptr);
    child3.setProperty("Highlight", false, nullptr);
    
    ValueTree grandchild2("Catagory");
    grandchild2.setProperty("Name", "Grandchild 2", nullptr);
    grandchild2.setProperty("Opened", true, nullptr);
    grandchild2.setProperty("Highlight", false, nullptr);
    
    child3.addChild(grandchild2, -1, nullptr);
    
    full.addChild(child3, -1, nullptr);*/
    
    testBrowser.setDataToDisplay(dataToAddTo.getChildWithName("Catagories"));
    
    addAndMakeVisible(testBrowser);
}

AddFilesParameterEditor::~AddFilesParameterEditor()
{
}

void AddFilesParameterEditor::paint (juce::Graphics& g)
{
    
}

void AddFilesParameterEditor::resized()
{
    titleLabel.setBounds(getWidth() / 3, 10, getWidth() / 3, 40);
    
    newVersionToggle.setBounds(getWidth() / 6 * 5, 50, 22, 20);
    
    if(!newVersionToggle.getToggleState())
    {
        testBrowser.setBounds(0, 80, getWidth(), getHeight() - 80);
    }
}

void AddFilesParameterEditor::buttonClicked(Button* button)
{
    if(button == &newVersionToggle)
    {
        repaint();
        
        juce::StringArray test = testBrowser.getAllHighlightedItems();
        
        std::for_each(test.begin(), test.end(), [](const juce::String& string)
        {
            DBG(string);
        });
    }
}
