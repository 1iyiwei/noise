/*
  MultiTree.hpp

  a set of trees where each tree stores samples from one class
  not really a forest

  Li-Yi Wei
  09/17/2008

*/

#ifndef _MULTI_TREE
#define _MULTI_TREE

#include <vector>
using namespace std;

#include "Tree.hpp"

class MultiTree
{
public:
    MultiTree(const DistanceField & distance_field, const ConflictChecker & conflict_checker); 
    ~MultiTree(void);

    int NumTrees(void) const;

    Tree & GetTree(const int which_tree);

    // operations analogous to those under Tree class
    int Conflict(const Sample & sample) const;

    int GetConflicts(const Sample & sample, vector<const Sample *> & neighbors) const;

    // int Impossible(const int class_id) const;

    int Add(const Sample & sample);
    int Remove(const Sample & sample);

    int AddDeadZone(const Sample & sample);
    int RemoveDeadZone(const Sample & sample);

    void GetSamples(vector<const Sample *> & samples) const;

protected:
    vector<Tree *> _trees;
};

#endif
