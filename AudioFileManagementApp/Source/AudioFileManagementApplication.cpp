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
    
    dataTree.addListener(this);
    
    categoryDataHandler = std::make_unique<CategoryDataFormatter>(dataTree.getChildWithName("Categories"));
    fxDataHandler = std::make_unique<FXDataFormatter>(dataTree.getChildWithName("FXList"));
    
    auto test = dataTree.getChildWithName("Categories").getChildWithProperty("Name", "Fruit");
    
    DBG(int(test.isValid()));
    
    test.removeChild(test.getChildWithProperty("Name", "Lemon"), nullptr);
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
            addFilesComponent = std::make_unique<AddFilesComponent>(dataTree, fxDataHandler.get());
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

void AudioFileManagementApplication::valueTreeChildRemoved(ValueTree& parent, ValueTree& removedChild, int index)
{
    if((parent == dataTree.getChildWithName("Categories") || parent.isAChildOf(dataTree.getChildWithName("Categories"))) && removedChild.getType().toString() == "Category")
    {
        fxDataHandler->deleteCategoryFromAllFX(removedChild.getProperty("IDNum"));
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
