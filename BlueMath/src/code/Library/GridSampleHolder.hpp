/*
  GridSampleHolder.hpp

  Li-Yi Wei
  04/26/2008

  09/21/2010: remove conflict check stuff, and purity it as a sample holder
*/


#ifndef _GRID_SAMPLE_HOLDER_HPP
#define _GRID_SAMPLE_HOLDER_HPP

#include "SampleHolder.hpp"
#include "Array.hpp"

class GridSampleHolder : public SampleHolder
{
public:
    struct DomainSpec
    {
        DomainSpec(void) : cell_size(0) {};
        DomainSpec(const vector<int> & domain_resolution, const float cell_size_i) : domain_size(domain_resolution), cell_size(cell_size_i) {};

        vector<int> domain_size; // discretized in units of cell_size
        float cell_size;
    };

    GridSampleHolder(const Domain & domain, const DomainSpec & spec);
    GridSampleHolder(const Domain & domain, const vector<int> & domain_resolution, const float cell_size);
    ~GridSampleHolder(void);

    int Dimension(void) const;
    vector<int> NumCells(void) const;
    float CellSize(void) const;

    int Inside(const Sample & sample) const;

    int Add(const Sample & sample);

    int Remove(const Sample & sample);

    void Clear(void);

    void GetSamples(vector<const Sample *> & samples) const;

    int GetNeighbors(const Sample & query, const float radius, vector<const Sample *> & neighbors) const;

    // get samples from a particular cell
    // return 1 if successful, 0 if error (e.g. cell not existing)
    int GetSamples(const vector<int> & cell_index, vector<const Sample *> & samples) const;

    // locate the cell that contains this sample
    // return 1 if such cell is found, 0 else (i.e. sample outside grid)
    int LocateCell(const Sample & sample, vector<int> & cell_index) const;
    
    static int CellOffset(const float min_distance, const float cell_size);

protected:
    static int CommonCheck(const Domain & domain, const DomainSpec & spec);

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
