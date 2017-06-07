/*
  PhaseGroup.hpp

  Compute phase groups

  Li-Yi Wei
  07/25/2007

*/

#ifndef _PHASE_GROUP_HPP
#define _PHASE_GROUP_HPP

#include <vector>
using namespace std;

#include "KDTree.hpp"

class PhaseGroup
{
public:
    typedef enum {SEQUENTIAL_RANDOM, PARALLEL_ONION, PARALLEL_RANDOM_TEMPLATE} Order;

    static int Compute(const Order order, KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups);

protected:
    static int ComputeSequentialRandom(KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups);

    static int ComputeParallelOnion(KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups);

    static int ComputeParallelRandomTemplate(KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups);
    
    static int MappingTableEntry(const int template_size, const vector<int> & index);
};

#endif
