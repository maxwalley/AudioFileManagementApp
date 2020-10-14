/*
  ==============================================================================

    FXDataFormatter.cpp
    Created: 6 Oct 2020 3:10:01pm
    Author:  Max Walley

  ==============================================================================
*/

#include "FXDataFormatter.h"

FXDataFormatter::FXDataFormatter(ValueTree fxData)  : ValueTreeIDNumberer(fxData, "FX", false)
{
    initialTreeFormat();
}

FXDataFormatter::~FXDataFormatter()
{
    
}

void FXDataFormatter::deleteCategoriesFromAllFX(const var& idNumToDelete)
{
    std::for_each(managedTree.begin(), managedTree.end(), [&idNumToDelete](ValueTree fx)
    {
        ValueTree categoryList = fx.getChildWithName("Categories");
        ValueTree childToDelete = categoryList.getChildWithProperty("IDNum", idNumToDelete);
        
        if(childToDelete.isValid())
        {
            categoryList.removeChild(childToDelete, nullptr);
        }
    });
}

void FXDataFormatter::deleteCategoriesFromAllFX(const std::vector<var>& idNumsToDelete)
{
    std::for_each(managedTree.begin(), managedTree.end(), [&idNumsToDelete](ValueTree fx)
    {
        ValueTree categoryList = fx.getChildWithName("Categories");
        
        std::for_each(idNumsToDelete.begin(), idNumsToDelete.end(), [&categoryList](const var& idNum)
        {
            ValueTree childToDelete = categoryList.getChildWithProperty("IDNum", idNum);
            
            if(childToDelete.isValid())
            {
                categoryList.removeChild(childToDelete, nullptr);
            }
        });
    });
}

void FXDataFormatter::childTreeAdded(ValueTree& parent, ValueTree& newChild)
{
    if(newChild.getType().toString() == "FX")
    {
        newChild.getOrCreateChildWithName("Categories", nullptr);
        newChild.getOrCreateChildWithName("Keywords", nullptr);
    }
}

void FXDataFormatter::initialTreeFormat(ValueTree treeToFormat)
{
    if(!treeToFormat.isValid())
    {
        treeToFormat = managedTree;
    }
    
    std::for_each(treeToFormat.begin(), treeToFormat.end(), [this](ValueTree tree)
    {
        if(tree.getType().toString() == "FX")
        {
            tree.getOrCreateChildWithName("Categories", nullptr);
            tree.getOrCreateChildWithName("Keywords", nullptr);
        
            if(tree.getNumChildren() > 2)
            {
                initialTreeFormat(tree);
            }
        }
    });
}
