/*
  KDTree.hpp

  the tree itself assumes toroidal boundary condition for its cells
  the root domain is [0 1]^n hypercube
  right now, I assume there is only one root node
  although the code supports multiple roots (i.e. a forest)

  I handle non-hypercube regions via proper DistanceField
  
  Li-Yi Wei
  07/11/2007

  TODO:
  add a distance field class

  if I traverse from the root, then it is a logarithmic operation
  and I might not need to track neighborhood cells at same level
*/

#ifndef _KD_TREE_HPP
#define _KD_TREE_HPP

#include <vector>
using namespace std;

#define _CONFLICT_HYPERCUBE 1
#define _CONFLICT_HYPERSPHERE 2
#define _CONFLICT_2POWERN 3
#define _CONFLICT_ALGORITHM _CONFLICT_HYPERSPHERE

#include "Sample.hpp"
#include "DistanceField.hpp"

#if _CONFLICT_ALGORITHM == _CONFLICT_HYPERCUBE
#include "SequentialCounter.hpp"
#elif _CONFLICT_ALGORITHM == _CONFLICT_HYPERSPHERE
#include "OnionLayerCounter.hpp"
#endif

class KDTree
{
public:
    class Cell
    {
    public:
        // child_index contains only 0 or 1 as it is relative to the parent
#if _CONFLICT_ALGORITHM == _CONFLICT_2POWERN
        Cell(KDTree & tree, Cell * parent, const vector<int> & child_index);
#else
        Cell(KDTree & tree, const Cell * parent, const vector<int> & child_index);
#endif
        ~Cell(void);

        int Level(void) const;
        float Size(void) const;
        int GetIndex(vector<int> & answer) const;

        int NumSamples(void) const;
        int NumChildren(void) const; // contain info for being leaf/internal

        const Cell * Parent(void) const;
        
        int GetSamples(vector<const Sample *> & samples) const;
        
        int GetChildren(vector<Cell *> & children);
        int GetChildren(vector<const Cell *> & children) const;

        // return 1 if successful, 0 else
        // should only be called by KDTree
        int Subdivide(void);

        // return 0 if the cell does not exist (e.g. due to adaptive sampling)
        // should only be called by KDTree
        const Cell * FindCell(const int level, const vector<int> & index) const;
    
        // return 1 if sample inside grid, 0 else
        int Inside(const Sample & sample) const;

        // assume the sample is within this cell
        // return 1 if sample has conflict with any other sample in the tree
        // 0 else
        // this sounds weird but is optimized for the ordinary usage
        // of testing a new sample uniformly drawn from this cell
        int EccentricConflict(const Sample & sample) const;

        // return 1 if successful, 0 else
        int Add(const Sample & sample);

    protected:
        // for hypercube/sphere, return 1
        // for 2powern, return ceil(log(6*sqrt(dimension*1.0))/log2)+1
        static int NumSampleLevels(const int dimension);
        
        // return 1 if successful, 0 else
        int AddToMeOnly(const Sample & sample);
        
    protected:
        KDTree & _tree; // access to tree for global information
#if _CONFLICT_ALGORITHM == _CONFLICT_2POWERN
        Cell * _parent;
#else
        const Cell * _parent;
#endif
        const int _level; // 0 means root
        vector<int> _index; // multi-dimensional counting of this cell
        // the size information is from the containing kd-tree
        vector< vector<const Sample *> > _samples; // stores samples at multiple child levels. _samples[0] stores my own child (at most 1), _samples[1] stores samples for immediate children, and so on.
                
        vector<Cell *> _children; // length 0 for leaf or 2^dim for internal
    };

public:
    // assuming the domain resides in the positive region of hyperspace
    KDTree(const int dimension, const DistanceField & distance_field);
    KDTree(const int dimension, const float cell_size, const DistanceField & distance_field);
    ~KDTree(void);

    const DistanceField & GetDistanceField(void) const;

#if _CONFLICT_ALGORITHM == _CONFLICT_HYPERCUBE || _CONFLICT_ALGORITHM == _CONFLICT_HYPERSPHERE
    Counter & GetCounter(void);
#endif
    
    int Dimension(void) const;
    int NumLevels(void) const;
    float CellSize(const int level) const;
    int NumCells(const int level) const; // get the real # of cells per level
    
    // this is assuming a uniform subdivision all the way
    int GetNumCellsPerDimension(const int level, vector<int> & answer) const;
    
    int GetCells(const int level, vector<Cell *> & cells);
    int GetCells(const int level, vector<const Cell *> & cells) const;
    
    // get the cell with (level, index)
    // useful for checking conflicts from neighboring cells
    // return 0 if the cell does not exist
    const Cell * FindCell(const int level, const vector<int> & index) const;
        
    void GetSamples(vector<const Sample *> & samples) const;

    int Subdivide(void);

protected:
    void BuildRootCells(const float cell_size);
    
protected:
    const int _dimension;
    const DistanceField & _distance_field;
    vector< vector<Cell *> > _cells_per_level;
    vector<float> _cell_size_per_level;
    vector< vector<int> > _num_cells_per_dimension_per_level;
#if _CONFLICT_ALGORITHM == _CONFLICT_HYPERCUBE
    SequentialCounter _counter;
#elif _CONFLICT_ALGORITHM == _CONFLICT_HYPERSPHERE
    OnionLayerCounter _counter;
#endif
};

#endif
