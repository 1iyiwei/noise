/*
  Grid.hpp

  Li-Yi Wei
  04/26/2008

*/


#ifndef _GRID_HPP
#define _GRID_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"
#include "Array.hpp"
#include "ConflictChecker.hpp"

class Grid
{
public:
    struct DomainSpec
    {
        vector<int> domain_size; // discretized in units of cell_size
        float cell_size;
    };

    // convenience routine to build a specific kind of domain_spec
    // with input float values that are mostly rational
    static DomainSpec BuildDomainSpec(const vector<float> & domain_spec);

    Grid(const DomainSpec & spec, const float min_distance);
    ~Grid(void);

    int Dimension(void) const;
    vector<int> NumCells(void) const;
    float CellSize(void) const;

    // return 1 if sample inside grid, 0 else
    int Inside(const Sample & sample) const;

    // return 1 if sample is in conflict with any existing samples, 0 else
    int Conflict(const Sample & sample, const ConflictChecker & checker) const;

    // return 1 if successful, 0 else
    // sample is not cloned and will hereafter belong to the grid
    int Add(const Sample & sample);

    // the inverse operation of Add
    // remove the sample from the grid, and regain ownership
    // return 1 if successful, 0 else
    int Remove(const Sample & sample);

    void GetSamples(vector<const Sample *> & samples) const;

    // collect all neighbors that are in conflict with sample
    // sample does not have to be attached to the grid
    // but it must be inside the grid domain
    // return 1 if successful, 0 else
    // (the emptiness of neighbors does not imply success or failure
    int GetConflicts(const Sample & sample, const ConflictChecker & checker, vector<const Sample *> & neighbors) const;

protected:
    static DomainSpec ComputeDomainSpec(const DomainSpec & input, const float min_distance);

    // locate the cell that contains this sample
    // return 1 if such cell is found, 0 else (i.e. sample outside grid)
    int LocateCell(const Sample & sample, vector<int> & cell_index) const;
    
    static int CellOffset(const float min_distance, const float cell_size);

    // if find_all is 0, will return when any conflict is found
    // mainly for acceleration for Conflict()
    int GetConflicts(const Sample & sample, const ConflictChecker & checker, vector<const Sample *> & neighbors, const int find_all) const;

protected:
    struct Cell
    {
        Cell(void);

        // sample content does not belong to cell
        vector<const Sample *> samples;
    };

    const DomainSpec _domain_spec;
    Array<Cell> _cells;
};

#endif
