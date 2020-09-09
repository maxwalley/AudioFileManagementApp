/*
  ==============================================================================

    ProjectInitialiser.cpp
    Created: 8 Sep 2020 4:19:57pm
    Author:  Max Walley

  ==============================================================================
*/

#include "ProjectFilesHandler.h"

ProjectFilesHandler::ProjectFilesHandler(const String& settingsDirectoryPath)
{
    String settingsFilePath = settingsDirectoryPath + "/ProjectSettings.xml";
    
    File settingsFile = settingsFilePath;
    
    std::unique_ptr<XmlElement> settingsXml = parseAndCheckFile(ProjectFile::projectSettingsXml, settingsFile);
    
    if(settingsXml == nullptr)
    {
        if(!createBlankFile(ProjectFile::projectSettingsXml, settingsFile))
        {
            JUCEApplicationBase::quit();
        }
        
        else
        {
            settingsXml = parseAndCheckFile(ProjectFile::projectSettingsXml, settingsFile);
        }
    }
}

ProjectFilesHandler::~ProjectFilesHandler()
{
    
}

bool ProjectFilesHandler::createBlankFile (ProjectFile typeOfFileToCreate, File fileToCreate)
{
    
    if(fileToCreate.existsAsFile())
    {
        fileToCreate.deleteFile();
    }
    
    if(fileToCreate.create().failed())
    {
        return false;
    }
    
    //Writes the specific XML to the new file
    createDefaultXmlForFile(typeOfFileToCreate).writeTo(fileToCreate);
    
    return true;
}

XmlElement ProjectFilesHandler::createDefaultXmlForFile (ProjectFile typeOfXmlToCreate)
{
    if(typeOfXmlToCreate == ProjectFile::projectSettingsXml)
    {
        XmlElement tree("Settings");
        
        //Project Data Path
        tree.createNewChildElement("Project Data Settings")->setAttribute("Project Data Path", (File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + "/AudioFileManagementApp/Project Data.xml"));
            
        return tree;
    }
    
    //Project data XML
    else
    {
        XmlElement tree("Data");
        
        return tree;
    }
}

std::unique_ptr<XmlElement> ProjectFilesHandler::parseAndCheckFile (ProjectFile typeOfFileToCheck, File fileToCheck)
{
    if(!fileToCheck.existsAsFile())
    {
        return nullptr;
    }
    
    std::unique_ptr<XmlElement> parsedXml = parseXML(fileToCheck);
    
    if(parsedXml == nullptr)
    {
        return nullptr;
    }
    
    if(typeOfFileToCheck == ProjectFile::projectSettingsXml)
    {
        XmlElement* testerXml = parsedXml->getChildByName("Project Data Settings");
        
        if(testerXml == nullptr || !testerXml->hasAttribute("Project Data Path"))
        {
            return nullptr;
        }
        
        return parsedXml;
    }
    
}
