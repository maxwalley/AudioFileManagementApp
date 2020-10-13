/*
  ==============================================================================

    DataFormatters.cpp
    Created: 6 Oct 2020 12:30:52pm
    Author:  Max Walley

  ==============================================================================
*/

#include "CategoryDataFormatter.h"

CategoryDataFormatter::CategoryDataFormatter(ValueTree dataTree)  : ValueTreeManager(dataTree, "Category", true)
{
    initialFormatOfTree();
}

CategoryDataFormatter::~CategoryDataFormatter()
{
    
}

void CategoryDataFormatter::deleteFXIDsFromAllCategories(const var& idToDelete, ValueTree treeToDeleteFrom)
{
    if(!treeToDeleteFrom.isValid())
    {
        treeToDeleteFrom = managedTree;
    }
    
    std::for_each(treeToDeleteFrom.begin(), treeToDeleteFrom.end(), [this, &idToDelete](ValueTree category)
    {
        ValueTree fxList = category.getChildWithName("FXList");
        
        ValueTree childToDelete = fxList.getChildWithProperty("IDNum", idToDelete);
        
        if(childToDelete.isValid())
        {
            fxList.removeChild(childToDelete, nullptr);
        }
        
        deleteFXIDsFromAllCategories(idToDelete, category);
    });
}

void CategoryDataFormatter::deleteFXIDsFromAllCategories(const std::vector<var>& idsToDelete, ValueTree treeToDeleteFrom)
{
    if(!treeToDeleteFrom.isValid())
    {
        treeToDeleteFrom = managedTree;
    }
    
    std::for_each(treeToDeleteFrom.begin(), treeToDeleteFrom.end(), [this, &idsToDelete](ValueTree category)
    {
        ValueTree fxList = category.getChildWithName("FXList");
        
        std::for_each(idsToDelete.begin(), idsToDelete.end(), [&fxList](const var& idNumToDelete)
        {
            ValueTree childToDelete = fxList.getChildWithProperty("IDNum", idNumToDelete);
            
            if(childToDelete.isValid())
            {
                fxList.removeChild(childToDelete, nullptr);
            }
        });
        
        std::for_each(category.begin(), category.end(), [this, &idsToDelete](ValueTree subCat)
        {
            deleteFXIDsFromAllCategories(idsToDelete, subCat);
        });
    });
}

void CategoryDataFormatter::childTreeAdded(ValueTree& parent, ValueTree& newChild)
{
    if(newChild.getType().toString() == "Category")
    {
        newChild.getOrCreateChildWithName("FXList", nullptr);
    }
}

void CategoryDataFormatter::initialFormatOfTree(ValueTree treeToFormat)
{
    if(!treeToFormat.isValid())
    {
        treeToFormat = managedTree;
    }
    
    std::for_each(treeToFormat.begin(), treeToFormat.end(), [this](ValueTree tree)
    {
        if(tree.getType().toString() == "Category")
        {
            tree.getOrCreateChildWithName("FXList", nullptr);
        }
        
        if(tree.getNumChildren() > 0)
        {
            initialFormatOfTree(tree);
        }
    });
}
