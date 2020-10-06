/*
  ==============================================================================

    FXDataFormatter.cpp
    Created: 6 Oct 2020 3:10:01pm
    Author:  Max Walley

  ==============================================================================
*/

#include "FXDataFormatter.h"

FXDataFormatter::FXDataFormatter(ValueTree fxData)  : ValueTreeManager(fxData)
{
    initialTreeFormat();
}

FXDataFormatter::~FXDataFormatter()
{
    
}

void FXDataFormatter::childTreeAdded(ValueTree& parent, ValueTree& newChild)
{
    if(newChild.getType().toString() == "FX")
    {
        newChild.addChild(ValueTree("Categories"), -1, nullptr);
        newChild.addChild(ValueTree("Keywords"), -1, nullptr);
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
        tree.getOrCreateChildWithName("Categories", nullptr);
        tree.getOrCreateChildWithName("Keywords", nullptr);
        
        if(tree.getNumChildren() > 2)
        {
            initialTreeFormat(tree);
        }
    });
}
