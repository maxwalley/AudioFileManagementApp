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
        std::vector<var> idsToDelete = ValueTreeManager::getAllIDNumsInTree(childWhichHasBeenRemoved, "Category");
        
        fxFormatter.deleteCategoriesFromAllFX(idsToDelete);
    }
    
    else if((parent == managedTree.getChildWithName("FXList") || parent.isAChildOf(managedTree.getChildWithName("FXList"))) && childWhichHasBeenRemoved.getType().toString() == "FX")
    {
        const var moving = childWhichHasBeenRemoved.getProperty("Moving");
        
        DBG(int(moving));
        
        if(!moving.isVoid() || moving)
        {
            childWhichHasBeenRemoved.removeProperty("Moving", nullptr);
            return;
        }
        
        //If we're not moving
        categoryFormatter.deleteFXIDsFromAllCategories(childWhichHasBeenRemoved.getProperty("IDNum"));
    }
}
