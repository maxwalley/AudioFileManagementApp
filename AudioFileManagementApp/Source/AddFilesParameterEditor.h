/*
  ==============================================================================

    AddFilesParameterEditor.h
    Created: 16 Sep 2020 10:53:00am
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "HierachicalListBrowser.h"

//==============================================================================
/*
*/
using namespace juce;

class AddFilesParameterEditor  : public Component,
                                 public Button::Listener
{
public:
    AddFilesParameterEditor(juce::ValueTree currentData);
    ~AddFilesParameterEditor() override;

    void paint (Graphics&) override;
    void resized() override;

private:
    
    void buttonClicked(Button* button) override;
    
    Label titleLabel;
    
    ToggleButton newVersionToggle;
    Label newVersionLabel;
    
    HierachicalListBrowser testBrowser;
    
    juce::ValueTree dataToAddTo;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AddFilesParameterEditor)
};
