/*
  ==============================================================================

    ProjectInitialiser.cpp
    Created: 8 Sep 2020 4:19:57pm
    Author:  Max Walley

  ==============================================================================
*/

#include "ProjectFilesInitialiser.h"

ProjectFilesInitialiser::ProjectFilesInitialiser(const String& settingsDirectoryPath)
{
    /*String settingsFilePath = settingsDirectoryPath + "/ProjectSettings.xml";
    
    File projectSettingsFile = settingsFilePath;
    
    projectSettingsXML = std::make_unique<XmlDocument>(projectSettingsFile);
    
    if(!projectSettingsFile.existsAsFile())
    {
        std::cout << "Project settings file not found, creating a new one at '" << settingsFilePath << "'\n";
        Result creationRes = projectSettingsFile.create();
        
        XmlElement test("Blank");
        test.writeTo(projectSettingsFile);
        
        if(!creationRes)
        {
            std::cout << "Creation of the settings file failed: " << creationRes.getErrorMessage().toStdString() << "\n";
            JUCEApplicationBase::quit();
        }
    }
    
    if(projectSettingsXML->getDocumentElement() == nullptr)
    {
        DBG(projectSettingsXML->getLastParseError());
    }*/
}

ProjectFilesInitialiser::~ProjectFilesInitialiser()
{
    
}

bool ProjectFilesInitialiser::createBlankFile (ProjectFile typeOfFileToCreate, File directory)
{
    File fileBeingCreated;
    
    switch (typeOfFileToCreate)
    {
        case ProjectFile::projectSettingsXml:
            fileBeingCreated = directory.getFullPathName() + "/ProjectSettings.xml";
            break;
            
        case ProjectFile::dataXml:
            fileBeingCreated = directory.getFullPathName() + "/ProjectData.xml";
    }
    
    if(fileBeingCreated.existsAsFile())
    {
        fileBeingCreated.deleteFile();
    }
    
    if(fileBeingCreated.create().failed())
    {
        return false;
    }
}

XmlElement ProjectFilesInitialiser::createDefaultXmlForFile (ProjectFile typeOfXmlToCreate)
{
    switch (typeOfXmlToCreate)
    {
        case ProjectFile::projectSettingsXml:
            XmlElement tree("Settings");
            tree.createNewChildElement("Project Data Path")->addTextElement(File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + "/AudioFileManagementApp/Project Data.xml");
            return tree;
            

        
    }
}
