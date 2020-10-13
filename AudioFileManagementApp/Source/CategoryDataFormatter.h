/*
  ==============================================================================

    DataFormatters.h
    Created: 6 Oct 2020 12:30:52pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ValueTreeManager.h"

using namespace juce;

class CategoryDataFormatter  : public ValueTreeManager
{
public:
    CategoryDataFormatter(ValueTree dataTree);
    ~CategoryDataFormatter();
    
    //Leave treeTodeleteFrom blank if you want it to run through the whole tree
    void deleteFXIDsFromAllCategories(const var& idToDelete, ValueTree treeToDeleteFrom = ValueTree());
    void deleteFXIDsFromAllCategories(const std::vector<var>& idsToDelete, ValueTree treeToDeleteFrom = ValueTree());
    
private:
    void childTreeAdded(ValueTree& parent, ValueTree& child) override;
    
    void initialFormatOfTree(ValueTree treeToFormat = ValueTree());
};
