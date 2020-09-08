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

class ProjectFilesInitialiser
{
public:
    enum class ProjectFile
    {
        projectSettingsXml,
        dataXml
    };
    
    ProjectFilesInitialiser (const String& settingsDirectoryPath);
    ~ProjectFilesInitialiser();
    
private:
    //Directory should not have actual file on the end
    bool createBlankFile (ProjectFile typeOfFileToCreate, File directory);
    
    XmlElement createDefaultXmlForFile (ProjectFile typeOfXmlToCreate);
    
    std::unique_ptr<XmlDocument> projectSettingsXML;
    
};
