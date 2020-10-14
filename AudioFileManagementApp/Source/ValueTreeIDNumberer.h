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

class ValueTreeIDNumberer   :  public ValueTree::Listener
{
public:
    ValueTreeIDNumberer(ValueTree treeToManage, Identifier childrenToAddIDsTo = Identifier(), bool addIDsRecursively = false);
    ~ValueTreeIDNumberer();
    
    //Leave treeToSearch blank if you want it to iterate through the managed tree
    ValueTree getTreeWithID(var idToLookFor, ValueTree treeToSearch = ValueTree()) const;
    
    //If treesToGetIDsFrom is blank this will just get IDs from all trees
    static std::vector<var> getAllIDNumsInTree(const ValueTree& treeToSearch, Identifier treesToGetIDsFrom = Identifier(), bool searchRootTreeforID = true);
    
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
