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
    dataTree = fileHandler.getTreeFromFile(ProjectFilesHandler::ProjectFile::projectData);
    dataManager = std::make_unique<DataTreeManager>(dataTree);
    
    mainComponent = std::make_unique<MainComponent>(dataTree);
    mainWindow.reset (new MainWindow (getApplicationName()));
    mainWindow->setContentNonOwned(mainComponent.get(), true);
    
    menu.addActionListener(this);
    MenuBarModel::setMacMainMenu(&menu);
}

void AudioFileManagementApplication::shutdown()
{
    dataTree.getChildWithName("ControlList").setProperty("DelNewFX", true, nullptr);
    
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
        dataTree.getChildWithName("FXList").getChildWithName("NewFiles").removeAllChildren(nullptr);
        
        if(addFilesWindow == nullptr)
        {
            addFilesWindow = std::make_unique<ComponentWindow>("Select Files to Add", Colours::silver, DocumentWindow::allButtons);
            
            addFilesWindow->addActionListener(this);
        }
        
        if(addFilesWindow->getContentComponent() == nullptr)
        {
            addFilesComponent = std::make_unique<AddFilesComponent>(dataTree);
            addFilesComponent->addListener(this);
            addFilesWindow->setContentNonOwned(addFilesComponent.get(), true);
        }
        
        FileChooser chooser("Select files to add");
        
        if(chooser.browseForMultipleFilesOrDirectories())
        {
            if(addFilesComponent->processAndAddFiles(chooser.getResults()))
            {
                addFilesWindow->setVisible(true);
            }
        }
    }
    
    else if(message == "Select Files to Add_closed")
    {
        dataTree.getChildWithName("ControlList").setProperty("DelNewFX", true, nullptr);
    }
}

void AudioFileManagementApplication::filesAdded(int numFilesAdded)
{
    if(numFilesAdded > 0)
    {
        //update some sort of display
    }
    
    if(addFilesComponent->getNumberOfFilesBeingAdded() == 0)
    {
        addFilesWindow->setVisible(false);
    }
}

AudioFileManagementApplication::MainWindow::MainWindow(juce::String name) : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);

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
