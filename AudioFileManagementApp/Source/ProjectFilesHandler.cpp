/*
  ==============================================================================

    ProjectInitialiser.cpp
    Created: 8 Sep 2020 4:19:57pm
    Author:  Max Walley

  ==============================================================================
*/

#include "ProjectFilesHandler.h"

ProjectFilesHandler::ProjectFilesHandler(const String& settingsDirectoryPath) : projectSettingsFile(settingsDirectoryPath + "/ProjectSettings.xml")
{
    std::optional<ValueTree> testSettingsTree;
    
    testSettingsTree = parseAndCheckFile(ProjectFile::projectSettings, projectSettingsFile);
    
    //Failed one of the tests
    if(testSettingsTree == std::nullopt)
    {
        if(!createBlankFile(ProjectFile::projectSettings, projectSettingsFile))
        {
            JUCEApplicationBase::quit();
        }
        
        else
        {
            testSettingsTree = parseAndCheckFile(ProjectFile::projectSettings, projectSettingsFile);
        }
    }
    
    projectSettingsTree = *testSettingsTree;
    
    
    //Opening the project data
    std::optional<ValueTree> testDataTree;
    
    String projectDataPath = projectSettingsTree.getChildWithName("ProjectData").getProperty("DataPath");
    
    projectDataFile = projectDataPath;
    
    testDataTree = parseAndCheckFile(ProjectFile::projectData, projectDataFile);
    
    //Failed one of the tests
    if(testDataTree == std::nullopt)
    {
        if(!createBlankFile(ProjectFile::projectData, projectDataFile))
        {
            JUCEApplicationBase::quit();
        }
        
        else
        {
            testDataTree = parseAndCheckFile(ProjectFile::projectData, projectDataFile);
        }
    }
    
    projectDataTree = *testDataTree;
}

ProjectFilesHandler::~ProjectFilesHandler()
{
    projectDataTree.createXml()->writeTo(projectDataFile);
    projectSettingsTree.createXml()->writeTo(projectSettingsFile);
}

ValueTree ProjectFilesHandler::getTreeFromFile(ProjectFile treeToReturn) const
{
    switch (treeToReturn)
    {
        case ProjectFile::projectSettings:
            return projectSettingsTree;
            
        case ProjectFile::projectData:
            return projectDataTree;
    }
}

bool ProjectFilesHandler::createBlankFile (ProjectFile typeOfFileToCreate, File fileToCreate)
{
    if(fileToCreate.existsAsFile())
    {
        fileToCreate.deleteFile();
    }
    
    if(fileToCreate.create().failed())
    {
        std::cout << "Failure to create file at path specified: " << fileToCreate.getFullPathName() << "\n";
        return false;
    }
    
    //Writes the specific XML to the new file
    createDefaultXmlForFile(typeOfFileToCreate).writeTo(fileToCreate);
    
    return true;
}

XmlElement ProjectFilesHandler::createDefaultXmlForFile (ProjectFile typeOfXmlToCreate) const
{
    if(typeOfXmlToCreate == ProjectFile::projectSettings)
    {
        XmlElement tree("Settings");
        
        //Project Data Path
        tree.createNewChildElement("ProjectData")->setAttribute("DataPath", (File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() + "/AudioFileManagementApp/Project Data.xml"));
            
        return tree;
    }
    
    //Project data XML
    else
    {
        XmlElement tree("Data");
        
        return tree;
    }
}

std::optional<ValueTree> ProjectFilesHandler::parseAndCheckFile (ProjectFile typeOfFileToCheck, File fileToCheck)
{
    if(!fileToCheck.existsAsFile())
    {
        return std::nullopt;
    }
    
    std::unique_ptr<XmlElement> parsedXml = parseXML(fileToCheck);
    
    if(parsedXml == nullptr)
    {
        return std::nullopt;
    }
    
    ValueTree treeFromData = ValueTree::fromXml(*parsedXml);
    
    if(typeOfFileToCheck == ProjectFile::projectSettings)
    {
        //Runs through the elements testing them
        ValueTree testerTree;
        
        testerTree = treeFromData.getChildWithName("ProjectData");
        
        //Doesn't have the element or the attribute
        if(!testerTree.isValid() || !testerTree.hasProperty("DataPath"))
        {
            return std::nullopt;
        }
        
        return treeFromData;
    }
    
    //Project data xml
    return treeFromData;
}
