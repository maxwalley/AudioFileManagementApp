/*
  ==============================================================================

    ProjectInitialiser.h
    Created: 8 Sep 2020 4:19:57pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

class ProjectFilesHandler
{
public:
    enum class ProjectFile
    {
        projectSettings,
        projectData
    };
    
    ProjectFilesHandler (const String& settingsDirectoryPath);
    ~ProjectFilesHandler();
    
    ValueTree getTreeFromFile(ProjectFile treeToReturn) const;
    
    void saveFile(ProjectFile fileToSave) const;
    
private:
    
    bool createBlankFile (ProjectFile typeOfFileToCreate, File fileToCreate);
    
    XmlElement createDefaultXmlForFile (ProjectFile typeOfXmlToCreate) const;
    
    //if file does have correct data returns the parsed xml
    std::optional<ValueTree> parseAndCheckFile (ProjectFile typeOfFileToCheck, File fileToCheck);
    
    File projectSettingsFile;
    File projectDataFile;
    
    ValueTree projectSettingsTree;
    ValueTree projectDataTree;
    
};
