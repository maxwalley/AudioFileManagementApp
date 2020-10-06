/*
  ==============================================================================

    AudioFileManagementApplication.h
    Created: 10 Sep 2020 12:12:42pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MainComponent.h"
#include "ProjectFilesHandler.h"
#include "MenuModel.h"
#include "ComponentWindow.h"
#include "AddFilesComponent.h"
#include "CategoryDataFormatter.h"
#include "FXDataFormatter.h"

class AudioFileManagementApplication  : public juce::JUCEApplication,
                                        public juce::ActionListener
{
public:
    
    AudioFileManagementApplication();
    
    const juce::String getApplicationName() override;
    const juce::String getApplicationVersion() override;
    bool moreThanOneInstanceAllowed() override;

    void initialise (const juce::String& commandLine) override;

    void shutdown() override;

    void systemRequestedQuit() override;

    void anotherInstanceStarted (const juce::String& commandLine) override;

    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name);

        void closeButtonPressed() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    MenuModel menu;
    ProjectFilesHandler fileHandler;
    std::unique_ptr<ComponentWindow> addFilesWindow;
    std::unique_ptr<AddFilesComponent> addFilesComponent;
    std::unique_ptr<CategoryDataFormatter> categoryDataHandler;
    std::unique_ptr<FXDataFormatter> fxDataHandler;
    
    ValueTree dataTree;
    
    TooltipWindow tooltipWindow;
    
    void actionListenerCallback(const juce::String& message) override;
    
};
