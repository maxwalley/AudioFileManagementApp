/*
  ==============================================================================

    CategoryDataHandler.cpp
    Created: 6 Oct 2020 9:51:45am
    Author:  Max Walley

  ==============================================================================
*/

#include "ValueTreeIDNumberer.h"

ValueTreeIDNumberer::ValueTreeIDNumberer(ValueTree treeToManage, Identifier childrenToAddIDsTo, bool addIDsRecursively)  : managedTree(treeToManage), childIDToAddNumsTo(childrenToAddIDsTo), addRecursively(addIDsRecursively), nextHighestIDNum(findNextHighestIDNum(managedTree))
{
    managedTree.addListener(this);
    
    addIDNumToTree(managedTree);
}

ValueTreeIDNumberer::~ValueTreeIDNumberer()
{
    
}

ValueTree ValueTreeIDNumberer::getTreeWithID(var idToLookFor, ValueTree treeToSearch) const
{
    ValueTree foundTree;

    if(!treeToSearch.isValid())
    {
        treeToSearch = managedTree;
    }
    
    for(const ValueTree& tree : treeToSearch)
    {
        if(childIDToAddNumsTo.isValid() && tree.getType() != childIDToAddNumsTo)
        {
            continue;
        }
        
        if(tree.getProperty("IDNum") == idToLookFor)
        {
            foundTree = tree;
        }
        
        else if(tree.getNumChildren() > 0)
        {
            ValueTree resFromChild = getTreeWithID(idToLookFor, tree);
            
            if(resFromChild.isValid())
            {
                foundTree = resFromChild;
            }
        }
        
        if(foundTree.isValid())
        {
            break;
        }
    }
    
    return foundTree;
}

std::vector<var> ValueTreeIDNumberer::getAllIDNumsInTree(const ValueTree& treeToSearch, Identifier treesToGetIDsFrom, bool searchRootTreeForID)
{
    std::vector<var> listOfIDs;
    
    if(searchRootTreeForID)
    {
        if(treeToSearch.hasProperty("IDNum") && (!treesToGetIDsFrom.isValid() || treeToSearch.getType() == treesToGetIDsFrom))
        {
            listOfIDs.push_back(treeToSearch.getProperty("IDNum"));
        }
    }
    
    std::for_each(treeToSearch.begin(), treeToSearch.end(), [&listOfIDs, &treesToGetIDsFrom](const ValueTree& child)
    {
        std::vector foundIDsInChild = getAllIDNumsInTree(child, treesToGetIDsFrom);
        
        listOfIDs.insert(listOfIDs.end(), foundIDsInChild.begin(), foundIDsInChild.end());
    });
    
    return listOfIDs;
}

void ValueTreeIDNumberer::valueTreeChildAdded(ValueTree& parent, ValueTree& childThatHasBeenAdded)
{
    addIDNumToTree(childThatHasBeenAdded);
    
    childTreeAdded(parent, childThatHasBeenAdded);
}

int ValueTreeIDNumberer::findNextHighestIDNum(ValueTree treeToSearch) const
{
    int highest = 0;
    
    for (const ValueTree& tree : treeToSearch)
    {
        if(childIDToAddNumsTo.isValid() && tree.getType() != childIDToAddNumsTo)
        {
            continue;
        }
        
        if(int(tree.getProperty("IDNum")) > highest)
        {
            highest = tree.getProperty("IDNum");
        }
        
        if(addRecursively && tree.getNumChildren() > 0)
        {
            int highestFromChild = findNextHighestIDNum(tree);
            
            if(highestFromChild > highest)
            {
                highest = highestFromChild - 1;
            }
        }
    }

    return highest + 1;
}

void ValueTreeIDNumberer::addIDNumToTree(ValueTree treeToAddTo)
{
    if(!childIDToAddNumsTo.isValid() || childIDToAddNumsTo == treeToAddTo.getType())
    {
        if(!treeToAddTo.hasProperty("IDNum"))
        {
            treeToAddTo.setProperty("IDNum", nextHighestIDNum++, nullptr);
        }
    }
    
    if(addRecursively && treeToAddTo.getNumChildren() > 0)
    {
        std::for_each(treeToAddTo.begin(), treeToAddTo.end(), [this](ValueTree tree)
        {
            addIDNumToTree(tree);
        });
    }
}
