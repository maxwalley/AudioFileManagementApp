/*
  ==============================================================================

    CategoryDataHandler.h
    Created: 6 Oct 2020 9:51:45am
    Author:  Max Walley

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

class ValueTreeManager   :  public ValueTree::Listener
{
public:
    ValueTreeManager(ValueTree treeToManage, Identifier childrenToAddIDsTo = Identifier(), bool addIDsRecursively = false);
    ~ValueTreeManager();
    
    //Leave treeToSearch blank if you want it to iterate through the managed tree
    ValueTree getTreeWithID(var idToLookFor, ValueTree treeToSearch = ValueTree()) const;
    
protected:
    virtual void childTreeAdded(ValueTree& parentTree, ValueTree& newChild){};
    
    ValueTree managedTree;
    
private:
    void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
    
    int findNextHighestIDNum(ValueTree treeToSearch) const;
    
    void addIDNumToTree(ValueTree treeToAddTo);
    
    Identifier childIDToAddNumsTo;
    bool addRecursively;
    int nextHighestIDNum;
};
