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

#include "Sample.hpp"
#include "DistanceField.hpp"
#include "OnionLayerCounter.hpp"
#include "NBallSliceCounter.hpp"

class KDTree
{
public:
    class Cell
    {
    public:
        // child_index contains only 0 or 1 as it is relative to the parent
        Cell(KDTree & tree, const Cell * parent, const vector<int> & child_index);
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
        int EccentricConflict(const Sample & sample, int & num_neighbors_checked) const;

        // return 1 if successful, 0 else
        int Add(const Sample & sample);

    protected:
        KDTree & _tree; // access to tree for global information
        const Cell * _parent;
        const int _level; // 0 means root
        vector<int> _index; // multi-dimensional counting of this cell
        // the size information is from the containing kd-tree
        vector<const Sample *> _samples; // at most 1, but use vector for generality
        vector<Cell *> _children; // length 0 for leaf or 2^dim for internal
    };

public:
    // assuming the domain resides in the positive region of hyperspace
    KDTree(const int dimension, const DistanceField & distance_field);
    KDTree(const int dimension, const float cell_size, const DistanceField & distance_field);
    ~KDTree(void);

    const DistanceField & GetDistanceField(void) const;

    Counter & GetCounter(void);
    
    int Dimension(void) const;
    int NumLevels(void) const;
    float CellSize(const int level) const;
    int NumCells(const int level) const; // get the real # of cells per level

    // 9*dimension for adaptive case and
    // (r/cellsize + sqrt(dimension))^2 for uniform case
    float ConflictRadiusSquare(void) const;
    
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

    static float ConflictRadiusSquare(const int dimension, const int uniform_distance_field);
    
protected:
    const int _dimension;
    const DistanceField & _distance_field;
    vector< vector<Cell *> > _cells_per_level;
    vector<float> _cell_size_per_level;
    vector< vector<int> > _num_cells_per_dimension_per_level;
    OnionLayerCounter _counter;
    //NBallSliceCounter _counter;
};

#endif
