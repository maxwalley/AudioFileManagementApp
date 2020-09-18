/*
  ==============================================================================

    AudioFileManagementApplication.cpp
    Created: 10 Sep 2020 12:12:42pm
    Author:  Max Walley

  ==============================================================================
*/

#include "AudioFileManagementApplication.h"

AudioFileManagementApplication::AudioFileManagementApplication() : fileHandler(File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName() + "/AudioFileManagementApp")
{
    
}

const juce::String AudioFileManagementApplication::getApplicationName()
{
    return ProjectInfo::projectName;
}

const juce::String AudioFileManagementApplication::getApplicationVersion()
{
    return ProjectInfo::versionString;
}

bool AudioFileManagementApplication::moreThanOneInstanceAllowed()
{
    return true;
}

void AudioFileManagementApplication::initialise (const juce::String& commandLine)
{
    mainWindow.reset (new MainWindow (getApplicationName()));
    
    menu.addActionListener(this);
    MenuBarModel::setMacMainMenu(&menu);
    
    dataTree = fileHandler.getTreeFromFile(ProjectFilesHandler::ProjectFile::projectData);
}

void AudioFileManagementApplication::shutdown()
{
    mainWindow = nullptr;
    
    MenuBarModel::setMacMainMenu(nullptr);
}

void AudioFileManagementApplication::systemRequestedQuit()
{
    quit();
}

void AudioFileManagementApplication::anotherInstanceStarted (const juce::String& commandLine)
{
    
}

void AudioFileManagementApplication::actionListenerCallback(const juce::String& message)
{
    if(message == "Add")
    {
        if(addFilesWindow == nullptr)
        {
            addFilesWindow = std::make_unique<ComponentWindow>("Select Files to Add", Colours::silver, DocumentWindow::allButtons);
        }
        
        if(addFilesWindow->getContentComponent() == nullptr)
        {
            addFilesComponent = std::make_unique<AddFilesComponent>(dataTree);
            addFilesWindow->setContentNonOwned(addFilesComponent.get(), true);
        }
        
        if(addFilesComponent->lookForFilesAndAdd())
        {
            addFilesWindow->setVisible(true);
        }
    }
}

AudioFileManagementApplication::MainWindow::MainWindow(juce::String name) : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
     setContentOwned (new MainComponent(), true);

    #if JUCE_IOS || JUCE_ANDROID
     setFullScreen (true);
    #else
     setResizable (true, true);
     centreWithSize (getWidth(), getHeight());
    #endif

     setVisible (true);
}

void AudioFileManagementApplication::MainWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}
