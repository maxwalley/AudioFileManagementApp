/*
  ==============================================================================

    MenuModel.cpp
    Created: 10 Sep 2020 12:43:10pm
    Author:  Max Walley

  ==============================================================================
*/

#include "MenuModel.h"

MenuModel::MenuModel()
{
    
}

MenuModel::~MenuModel()
{
    
}

StringArray MenuModel::getMenuBarNames()
{
    return StringArray{"File"};
}

PopupMenu MenuModel::getMenuForIndex (int topLevelMenuIndex, const juce::String &menuName)
{
    PopupMenu menu;
    
    if(topLevelMenuIndex == 0)
    {
        menu.addItem(1, "Add Files");
    }
    
    return menu;
}

void MenuModel::menuItemSelected (int menuItemID, int topLevelMenuIndex)
{
    if(menuItemID == 1)
    {
        sendActionMessage("Add");
    }
}
