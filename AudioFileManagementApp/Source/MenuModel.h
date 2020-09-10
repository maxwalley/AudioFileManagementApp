/*
  ==============================================================================

    MenuModel.h
    Created: 10 Sep 2020 12:43:10pm
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

class MenuModel : public MenuBarModel,
                  public ActionBroadcaster
{
public:
    MenuModel();
    ~MenuModel();
    
    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex (int topLevelMenuIndex, const String &menuName) override;
    void menuItemSelected (int menuItemID, int topLevelMenuIndex) override;
    
private:
    
};
