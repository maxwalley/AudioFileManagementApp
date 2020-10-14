/*
  ==============================================================================

    DataTreeManager.cpp
    Created: 13 Oct 2020 3:43:55pm
    Author:  Max Walley

  ==============================================================================
*/

#include "DataTreeManager.h"

DataTreeManager::DataTreeManager(ValueTree& treeToManage) : managedTree(treeToManage), fxFormatter(managedTree.getChildWithName("FXList")), categoryFormatter(managedTree.getChildWithName("Categories"))
{
    managedTree.addListener(this);
}

DataTreeManager::~DataTreeManager()
{
    
}

void DataTreeManager::valueTreeChildRemoved(ValueTree& parent, ValueTree& childWhichHasBeenRemoved, int index)
{
    if((parent == managedTree.getChildWithName("Categories") || parent.isAChildOf(managedTree.getChildWithName("Categories"))) && childWhichHasBeenRemoved.getType().toString() == "Category")
    {
        std::vector<var> idsToDelete = ValueTreeIDNumberer::getAllIDNumsInTree(childWhichHasBeenRemoved, "Category");
        
        fxFormatter.deleteCategoriesFromAllFX(idsToDelete);
    }
    
    else if((parent == managedTree.getChildWithName("FXList") || parent.isAChildOf(managedTree.getChildWithName("FXList"))) && childWhichHasBeenRemoved.getType().toString() == "FX")
    {
        const var moving = childWhichHasBeenRemoved.getProperty("Moving");
        
        if(!moving.isVoid() || moving)
        {
            childWhichHasBeenRemoved.removeProperty("Moving", nullptr);
            return;
        }
        
        //If we're not moving
        categoryFormatter.deleteFXIDsFromAllCategories(childWhichHasBeenRemoved.getProperty("IDNum"));
    }
}

void DataTreeManager::valueTreePropertyChanged(ValueTree& treeWhosePropertyChanged, const Identifier& property)
{
    if(treeWhosePropertyChanged.getType().toString() == "ControlList" && treeWhosePropertyChanged.getParent() == managedTree)
    {
        if(property.toString() == "DelNewFX")
        {
            if(treeWhosePropertyChanged.getProperty(property))
            {
                DBG("WIPING NEW");
                
                managedTree.getChildWithName("FXList").getChildWithName("NewFiles").removeAllChildren(nullptr);
                
                treeWhosePropertyChanged.setPropertyExcludingListener(this, property, false, nullptr);
            }
        }
    }
}
