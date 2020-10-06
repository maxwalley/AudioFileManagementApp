/*
  ==============================================================================

    DataFormatters.cpp
    Created: 6 Oct 2020 12:30:52pm
    Author:  Max Walley

  ==============================================================================
*/

#include "DataFormatters.h"

CategoryDataFormatter::CategoryDataFormatter(ValueTree dataTree)  : ValueTreeManager(dataTree, "Catagory", true)
{
    initialFormatOfTree();
}

CategoryDataFormatter::~CategoryDataFormatter()
{
    
}

void CategoryDataFormatter::childTreeAdded(ValueTree& parent, ValueTree& newChild)
{
    if(newChild.getType().toString() != "FXList")
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
        if(tree.getType().toString() == "Catagory")
        {
            tree.getOrCreateChildWithName("FXList", nullptr);
        }
        
        if(tree.getNumChildren() > 0)
        {
            initialFormatOfTree(tree);
        }
    });
}
