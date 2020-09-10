/*
  ==============================================================================

    AddFilesComponent.h
    Created: 10 Sep 2020 2:26:58pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
using namespace juce;

class AddFilesComponent  : public juce::Component
{
public:
    AddFilesComponent();
    ~AddFilesComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    //Opens a file browser and adds the files to the component
    //Returns whether valid files were selected
    bool lookForFilesAndAdd();

private:
    Array<File> filesToAdd;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddFilesComponent)
};
