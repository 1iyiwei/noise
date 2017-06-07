/*
  Grid.hpp

  Li-Yi Wei
  06/28/2007

*/

#ifndef _GRID_HPP
#define _GRID_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"

class Grid
{
public:
    struct Cell
    {
        Cell(void);

        // sample content do not belong to cell
        const Sample * sample;
    };

    // idealized version
    static int CellOffset(const int dimension);
    static int CliqueSize(const int dimension);

    // practical version
    static int CellOffset(const int dimension, const float min_distance, const float cell_size);
    static int CliqueSize(const int dimension, const float min_distance, const float cell_size);
    
public:
    Grid(const int dimension, const float min_distance, const int clique_size);
    
    // return 1 if sample inside grid, 0 else
    int Inside(const Sample & sample) const;

    // locate the cell that contains this sample
    // return 1 if such cell is found, 0 else (i.e. sample outside grid)
    int LocateCell(const Sample & sample, vector<int> & cell_index) const;
    
    Cell * GetCell(const vector<int> & index);    
    const Cell * GetCell(const vector<int> & index) const;
    
    // return 1 if sample is within min_distance of an existing grid sample
    // 0 else
    int Conflict(const Sample & sample, const float min_distance) const;

    // return 1 if successful, 0 else
    int Add(const Sample & sample);

    void GetSamples(vector<const Sample *> & samples) const;
    
    int Dimension(void) const;
    
    int NumCellsPerDimension(void) const;
    
    float CellSize(void) const;

    int CellOffset(const float min_distance) const;
    int CliqueSize(const float min_distance) const;
    
protected:

    // return -1 if error
    int ConvertCellIndex(const vector<int> & index) const;
    
    static int P2(const int value);

protected:
    const int _dim;
    const int _num_cells_per_dim;
    const float _cell_size;
    vector<Cell> _cells;
};

#endif
