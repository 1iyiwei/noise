/*
  Tree.hpp

  tree data structure for multi-resolution multi-class Poisson disk sampling
  a hybrid from RainbowNoise/Grid class and BlueNoise/KDTree class

  Li-Yi Wei
  07/09/2008

*/

#ifndef _TREE_HPP
#define _TREE_HPP

#include <vector>
using namespace std;

#include "DistanceField.hpp"
#include "Array.hpp"
#include "ConflictChecker.hpp"
// #include "OnionLayerCounter.hpp" // don't worry about acceleration for now
#include "Grid.hpp" // for sharing defined data structures

class Tree
{
public:
    class Cell; // below

    typedef Grid::DomainSpec DomainSpec;

    Tree(const DistanceField & distance_field, const ConflictChecker & conflict_checker); 
    Tree(const DistanceField & distance_field, const ConflictChecker & conflict_checker, const DomainSpec & spec); // this version which allows explicit domain spec for the root level, is mostly for debug
    ~Tree(void);

    const DistanceField & GetDistanceField(void) const;
    const ConflictChecker & GetConflictChecker(void) const;

    int Dimension(void) const;
    int NumLevels(void) const;
    float CellSize(const int level) const;
    int NumCells(const int level) const; // get the real # of cells per level

    // this is assuming a uniform subdivision all the way
    int GetNumCellsPerDimension(const int level, vector<int> & answer) const;

    int Conflict(const Sample & sample) const;
    // would sample belong to the same tree or not?
    int Conflict(const Sample & sample, const int same_tree) const;

    // return 1 if successful, 0 else
    // sample is not cloned and will hereafter belong to the tree
    int Add(const Sample & sample);

    // the inverse operation of Add
    // remove the sample from the tree
    // and regain ownership for the caller
    // return 1 if successful, 0 else
    int Remove(const Sample & sample);

    // update the dead zones after adding/removing a sample
    // notice that the sample might not belong to the tree at all
    // (a useful feature for cross-tree update)
    int AddDeadZone(const Sample & sample);
    int RemoveDeadZone(const Sample & sample);
    
    // get the visit count for still possible leaf nodes for a particular class
    int GetVisitCount(const int class_id) const;

    int GetCells(const int level, vector<Cell *> & cells);
    int GetCells(const int level, vector<const Cell *> & cells) const;

protected:
    // warning: these could be very slow due to dynamic memory allocation
    int GetPossibleCells(const int class_id, vector<Cell *> & cells);
    int GetPossibleCells(const int class_id, vector<const Cell *> & cells) const;
public:

    // use these instead
    int NumPossibleCells(const int class_id) const;
    const Cell * GetPossibleCell(const int class_id, const int which_one) const;

    int NumLeafCells(void) const;
    const Cell * GetLeafCell(const int which_one) const;
    Cell * GetLeafCell(const int which_one);

    // get the cell with (level, index)
    // useful for checking conflicts from neighboring cells
    // return 0 if the cell does not exist
    const Cell * FindCell(const int level, const vector<int> & index) const;
        
    void GetSamples(vector<const Sample *> & samples) const;

    int GetConflicts(const Sample & sample, vector<const Sample *> & neighbors) const;
    int GetConflicts(const Sample & sample, const int same_tree, vector<const Sample *> & neighbors) const;

    int Subdivide(void);
    // mostly for debugging; blind subdivide would always divide
    int Subdivide(const int blind);

    int ConflictMetricFromChecker(void) const;

protected:    
    void BuildRootCells(const DomainSpec & spec);
    
    // locate the cell that contains this sample
    // return 1 if such cell is found, 0 else (i.e. sample outside grid)
    int LocateCell(const Sample & sample, int & level, vector<int> & cell_index) const;
    // related to above, but take a step further by translating cell_index into Cell
    const Cell * LocateCell(const Sample & sample, int & level) const;
    Cell * LocateCell(const Sample & sample, int & level);

    int CellOffset(void) const;
    static int CellOffset(const int dimension, const ConflictChecker & checker);
    static int CellOffset(const float min_distance, const float cell_size);

    int InitPoolsWithEmptyLeaves(void);

protected:
    class Pool // keeping track of available cells for each sample class
    {
    public:
        Pool(void);
        ~Pool(void);

    public:
        class CellLink // insert this for each cell
        {
        public:
            CellLink(const Cell * cell);
            ~CellLink(void);

            friend class Pool;

        protected:
            const Cell * cell;
            int position;
        };

        int Add(CellLink & link);
        int Remove(CellLink & link);
        void Clear(void);

        int GetCells(vector<const Cell *> & cells) const;

        int NumCells(void) const;
        const Cell * GetCell(const int which_cell) const;

        // total visit count of cells belonging to this pool
        int GetVisitCount(void) const;
        int AddVisitCount(const CellLink & link); // should only be called by Tree::AddVisitCount()

    protected:
        vector<CellLink *> _links;
        int _visit_count;
    };

public:
    class Cell // hyper-cube shaped
    {
    public:
        // child_index contains only 0 or 1 as it is relative to the parent
        Cell(Tree & tree, const Cell * parent, const vector<int> & child_index);
        ~Cell(void);

        int Level(void) const;
        float Size(void) const;
        const vector<int> & Index(void) const;
        void GetDomain(vector<float> & min_corner, vector<float> & max_corner) const;

        int NumSamples(void) const;
        int NumChildren(void) const; // contain info for being leaf/internal

        const Cell * Parent(void) const;
        
        // should only be called by Tree
        // find a cell with the particular index at the specified level
        // within the tree rooted at this cell
        // return the cell pointer if found, 0 else
        const Cell * FindCell(const int level, const vector<int> & index) const;
    
        // see the one under Tree
        int LocateCell(const Sample & sample, int & level, vector<int> & cell_index) const;
    
        // return 1 if sample inside cell, 0 else
        int Inside(const Sample & sample) const;

        int CellOffset(void) const;

        // return 1 if sample is in conflict with any existing samples, 0 else
        // worst_cast_distance is used to determine how many neighbor cells to look
        // (it is usually the upper bound of all r values)
        // assume the sample is within this cell
        // this sounds weird but is optimized for the ordinary usage
        // of testing a new sample uniformly drawn from this cell
        int Conflict(const Sample & sample) const;
        int Conflict(const Sample & sample, const int same_tree) const;

        // return 1 if successful, 0 else
        // sample is not cloned and will hereafter belong to the cell
        int Add(const Sample & sample);

        // the inverse operation of Add
        // remove the sample from the cell
        // and regain ownership for the caller
        // return 1 if successful, 0 else
        int Remove(const Sample & sample);

        // update the dead zones after adding/removing a sample
        // notice that the sample might not belong to the tree at all
        // (a useful feature for cross-tree update)
        int AddDeadZone(const Sample & sample);
        int RemoveDeadZone(const Sample & sample);

        // is it impossible to add another sample for class_id
        int Impossible(const int class_id) const;

        // visit count stuff
        int AddVisitCount(void);
        int GetVisitCount(void) const;

        int GetSamples(vector<const Sample *> & samples) const;
        // get all samples under the subtree rooted at this cell
        int GetSubTreeSamples(vector<const Sample *> & samples) const;

        // collect all neighbors that are in conflict with sample
        // sample does not have to be attached to the grid
        // but it must be inside the grid domain
        // return 1 if successful, 0 else
        // (the emptiness of neighbors does not imply success or failure
        // similar to conflict, assume sample is within this cell
        int GetConflicts(const Sample & sample, vector<const Sample *> & neighbors) const;
        int GetConflicts(const Sample & sample, const int same_tree, vector<const Sample *> & neighbors) const;

        int GetChildren(vector<Cell *> & children);
        int GetChildren(vector<const Cell *> & children) const;

        // return 1 if successful, 0 else
        // should only be called by Tree
        int Subdivide(void);
        int Subdivide(const int blind);

        // subdivide above does not migrate samples
        // which is done by the function below
        // Add() needs to have all the cells available to update conflict info
        // should only be called by Tree
        int Inherit(void);

        // should only be called by Tree::InitPoolWithEmptyLeaves
        int PoolAdd(const int class_id);
        int PoolRemove(const int class_id);

    protected: 
        // assuming sample is inside, but not necessarily belong to, the cell
        int GetConflicts(const Sample & sample, vector<const Sample *> & neighbors, const int find_all) const;
        int GetConflicts(const Sample & sample, const int same_tree, vector<const Sample *> & neighbors, const int find_all) const;

        // find the corner of this cell most far away from the query sample
        // used to determine if a cell is dead-zoned for a class by this sample 
        int FarCorner(const Sample & sample, Coordinate & corner) const;

        // determine, for each class,
        // whether the sample would make this entire cell 
        // impossible to sample from
        // the number of classes and their ids are implicit from the input array
        int DeadZone(const Sample & sample, vector<int> verdict_per_class) const;

        // update dead zones for relevant nearby cells
        // add_or_remove: +1 for add, -1 for remove
        int UpdateDeadZone(const Sample & sample, const int add_or_remove);

    protected:

        class MultiCounter
        {
        public:
            MultiCounter(void);
            ~MultiCounter(void);

            // return the new value
            int Increment(const int class_id);
            int Decrement(const int class_id);

            int Value(const int class_id) const;

        protected:
            vector<int> _counts;
        };

    protected:
        Tree & _tree; // access to tree for global information
        const Cell * _parent;
        const int _level; // 0 means root
        vector<int> _index; // multi-dimensional counting of this cell
        // the size information is from the containing kd-tree
        vector<const Sample *> _samples; // at most 1, but use vector for generality
        vector<Cell *> _children; // length 0 for leaf or 2^dim for internal
        MultiCounter _conflict_counter; // for keeping track of how many existing samples declare this entire cell dead zone for a particular class; only meaningful for leaf nodes
        vector<Pool::CellLink> _pool_links;

        int _visit_count; // count the number of times this cell has been visited
    };

public:
    // should be called only by Cell::PoolAdd()
    int PoolAdd(const int class_id, Pool::CellLink & link);
    // should be called only by Cell::PoolRemove()
    int PoolRemove(const int class_id, Pool::CellLink & link);
    // should be called only by Cell:AddVisitCount()
    int AddVisitCount(const int class_id, const Pool::CellLink & link);

protected:
    const DistanceField & _distance_field;
    const ConflictChecker & _conflict_checker;
    const int _cell_offset;
    const int _conflict_metric_from_checker;
    vector< vector<Cell *> > _cells_per_level; // collection of cells per level
    vector<float> _cell_size_per_level;
    vector< vector<int> > _num_cells_per_dimension_per_level;
    // OnionLayerCounter _counter;
    vector<Pool> _pools;
};

#endif
