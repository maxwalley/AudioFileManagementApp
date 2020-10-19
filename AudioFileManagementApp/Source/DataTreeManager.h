/*
  ==============================================================================

    DataTreeManager.h
    Created: 13 Oct 2020 3:43:55pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CategoryDataFormatter.h"
#include "FXDataFormatter.h"

using namespace juce;

class DataTreeManager : ValueTree::Listener
{
public:
    
    enum class TreeType
    {
        FXTree,
        CategoryTree
    };
    
    DataTreeManager(ValueTree& treeToManage);
    ~DataTreeManager();
    
    ValueTreeIDNumberer* getTreeFormatter(TreeType typeOfFormatter);
    
protected:
    void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
    
    void valueTreePropertyChanged(ValueTree& treeWhosePropertyChanged, const Identifier& property) override;
    
    ValueTree getManagedTree() const;
    
private:

    ValueTree managedTree;
    
    FXDataFormatter fxFormatter;
    CategoryDataFormatter categoryFormatter;
};
