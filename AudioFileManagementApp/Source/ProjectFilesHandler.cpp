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
    saveFile(ProjectFile::projectData);
    saveFile(ProjectFile::projectSettings);
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

void ProjectFilesHandler::saveFile(ProjectFile fileToSave) const
{
    if(fileToSave == ProjectFile::projectSettings)
    {
        projectSettingsTree.createXml()->writeTo(projectSettingsFile);
    }
    else if(fileToSave == ProjectFile::projectData)
    {
        projectDataTree.createXml()->writeTo(projectDataFile);
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
        tree.createNewChildElement("Categories");
        tree.createNewChildElement("FXList");
        
        XmlElement* test = tree.getChildByName("Categories");
        
        auto vehiclesChild = test->createNewChildElement("Category");
        vehiclesChild->setAttribute("Name", "Vehicles");
        vehiclesChild->setAttribute("IDNum", 1);
        
        auto carSubChild = vehiclesChild->createNewChildElement("Category");
        carSubChild->setAttribute("Name", "Car");
        carSubChild->setAttribute("IDNum", 2);
        
        auto bikeSubChild = vehiclesChild->createNewChildElement("Category");
        bikeSubChild->setAttribute("Name", "Bike");
        bikeSubChild->setAttribute("IDNum", 3);
        
        auto boatSubChild = vehiclesChild->createNewChildElement("Category");
        boatSubChild->setAttribute("Name", "Boat");
        boatSubChild->setAttribute("IDNum", 4);
        
        
        auto animalsChild = test->createNewChildElement("Category");
        animalsChild->setAttribute("Name", "Animals");
        animalsChild->setAttribute("IDNum", 5);
        
        auto catSubChild = animalsChild->createNewChildElement("Category");
        catSubChild->setAttribute("Name", "Cat");
        catSubChild->setAttribute("IDNum", 6);
        
        auto greyhoundSubChild = animalsChild->createNewChildElement("Category");
        greyhoundSubChild->setAttribute("Name", "Greyhound");
        greyhoundSubChild->setAttribute("IDNum", 7);
        
        
        auto fruitChild = test->createNewChildElement("Category");
        fruitChild->setAttribute("Name", "Fruit");
        fruitChild->setAttribute("IDNum", 8);
        
        auto lemonSubChild = fruitChild->createNewChildElement("Category");
        lemonSubChild->setAttribute("Name", "Lemon");
        lemonSubChild->setAttribute("IDNum", 9);
        
        auto berrySubChild = fruitChild->createNewChildElement("Category");
        berrySubChild->setAttribute("Name", "Berry");
        berrySubChild->setAttribute("IDNum", 10);
        
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
    
    //Runs through the elements testing them
    ValueTree testerTree;
    
    if(typeOfFileToCheck == ProjectFile::projectSettings)
    {
        testerTree = treeFromData.getChildWithName("ProjectData");
        
        //Doesn't have the element or the attribute
        if(!testerTree.isValid() || !testerTree.hasProperty("DataPath"))
        {
            return std::nullopt;
        }
        
        return treeFromData;
    }
    
    //Project Data XML
    
    //if it doesnt have these two children
    if(!treeFromData.getChildWithName("Categories").isValid() || !treeFromData.getChildWithName("FXList").isValid())
    {
        return std::nullopt;
    }
    
    //Project data xml
    return treeFromData;
}
