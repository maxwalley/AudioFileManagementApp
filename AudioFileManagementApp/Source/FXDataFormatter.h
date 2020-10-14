/*
  ==============================================================================

    FXDataFormatter.h
    Created: 6 Oct 2020 3:10:01pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ValueTreeIDNumberer.h"

using namespace juce;

class FXDataFormatter  : public ValueTreeIDNumberer
{
public:
    FXDataFormatter(ValueTree fxData);
    ~FXDataFormatter();
    
    void deleteCategoriesFromAllFX(const var& idNumToDelete);
    void deleteCategoriesFromAllFX(const std::vector<var>& idNumsToDelete);
    
private:
    void childTreeAdded(ValueTree& parent, ValueTree& child) override;
    
    void initialTreeFormat(ValueTree treeToFormat = ValueTree());
};
