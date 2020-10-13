/*
  ==============================================================================

    FXDataFormatter.h
    Created: 6 Oct 2020 3:10:01pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ValueTreeManager.h"

using namespace juce;

class FXDataFormatter  : public ValueTreeManager
{
public:
    FXDataFormatter(ValueTree fxData);
    ~FXDataFormatter();
    
    void deleteCategoryFromAllFX(const var& idNumToDelete);
    
private:
    void childTreeAdded(ValueTree& parent, ValueTree& child) override;
    
    void initialTreeFormat(ValueTree treeToFormat = ValueTree());
};
