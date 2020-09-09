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
        projectSettingsXml,
        dataXml
    };
    
    ProjectFilesHandler (const String& settingsDirectoryPath);
    ~ProjectFilesHandler();
    
private:
    
    bool createBlankFile (ProjectFile typeOfFileToCreate, File fileToCreate);
    
    XmlElement createDefaultXmlForFile (ProjectFile typeOfXmlToCreate);
    
    //if file does have correct data returns the parsed xml
    std::unique_ptr<XmlElement> parseAndCheckFile (ProjectFile typeOfFileToCheck, File fileToCheck);
    
    std::unique_ptr<XmlDocument> projectSettingsXML;
    
};
