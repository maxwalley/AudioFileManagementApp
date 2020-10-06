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
    
private:
    void childTreeAdded(ValueTree& parent, ValueTree& child) override;
    
    void initialFormatOfTree(ValueTree treeToFormat = ValueTree());
};
