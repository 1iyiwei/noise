/*
  GridSampleHolder.cpp

  Li-Yi Wei
  04/26/2008

  09/21/2010: remove conflict check stuff, and purity it as a sample holder
*/

#include <math.h>

#include "GridSampleHolder.hpp"
#include "Exception.hpp"
#include "SequentialCounter.hpp"
#include "NBallSliceCounter.hpp"
#include "Math.hpp"

#define _ALLOW_MULTIPLE_SAMPLES_PER_CELL

GridSampleHolder::Cell::Cell(void)
{
    // nothing to do
}

GridSampleHolder::GridSampleHolder(const Domain & domain, const DomainSpec & spec) : SampleHolder(domain), _domain_spec(spec), _cells(_domain_spec.domain_size)
{
    if(! CommonCheck(_domain, _domain_spec))
    {
        throw Exception("GridSampleHolder::GridSampleHolder(): illegal domain");
    }
}

GridSampleHolder::GridSampleHolder(const Domain & domain, const vector<int> & domain_resolution, const float cell_size) : SampleHolder(domain), _domain_spec(domain_resolution, cell_size), _cells(_domain_spec.domain_size)
{
    if(! CommonCheck(_domain, _domain_spec))
    {
        throw Exception("GridSampleHolder::GridSampleHolder(): illegal domain");
    }
}

int GridSampleHolder::CommonCheck(const Domain & domain, const DomainSpec & spec)
{
    vector<float> domain_size;
    if(! domain.BoundingBox(domain_size))
    {
        return 0;
    }

    if(domain_size.size() != spec.domain_size.size())
    {
        return 0;
    }

#if 0
    // seems to be overkill and could fail due to floating point precision
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        if(domain_size[i] > (spec.domain_size[i]*spec.cell_size))
        {
            return 0;
        }
    }
#endif
    return 1;
}

GridSampleHolder::~GridSampleHolder(void)
{
    // nothing to do
}

int GridSampleHolder::Dimension(void) const
{
    return _domain_spec.domain_size.size();
}

vector<int> GridSampleHolder::NumCells(void) const
{
    return _domain_spec.domain_size;
}

float GridSampleHolder::CellSize(void) const
{
    return _domain_spec.cell_size;
}

int GridSampleHolder::Inside(const Sample & sample) const
{
#if 1
    return _domain.Inside(sample);
#else
    if(sample.coordinate.Dimension() != Dimension())
    {
        throw Exception("GridSampleHolder::Inside(): sample dimension mismatch");
    }

    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        if((sample.coordinate[i] <= 0) || (sample.coordinate[i] >= (_domain_spec.domain_size[i]*_domain_spec.cell_size)))
        {
            return 0;
        }
    }

    return 1;
#endif
}

int GridSampleHolder::Add(const Sample & sample)
{
    if(sample.coordinate.Dimension() != Dimension()) return 0;

    vector<int> cell_index(Dimension());
    
    if(! LocateCell(sample, cell_index))
    {
        return 0;
    }
            
    Cell cell;
    if(!_cells.Get(cell_index, cell))
    {
        return 0;
    }
    
#ifndef _ALLOW_MULTIPLE_SAMPLES_PER_CELL
    if(cell.samples.size() > 0)
    {
        return 0;
    }
    else
#endif
    {
        cell.samples.push_back(&sample);
        if(!_cells.Put(cell_index, cell)) return 0;
        return 1;
    }
}

int GridSampleHolder::Remove(const Sample & sample)
{
    vector<int> cell_index(Dimension());
    
    if(! LocateCell(sample, cell_index))
    {
        return 0;
    }
            
    Cell cell;
    if(!_cells.Get(cell_index, cell))
    {
        return 0;
    }
    
    int found = 0;
    for(unsigned int i = 0; i < cell.samples.size(); i++)
    {
        if(cell.samples[i] == &sample)
        {
            found = 1;
            cell.samples[i] = cell.samples[cell.samples.size()-1];
            cell.samples.pop_back();
            break;
        }
    }

    if(! found)
    {
        return 0;
    }
    else
    {
        if(!_cells.Put(cell_index, cell)) return 0;
        return 1;
    }
}

void GridSampleHolder::Clear(void)
{
    vector<Cell> cells;
    _cells.Get(cells);

    for(unsigned int i = 0; i < cells.size(); i++)
    {
        cells[i].samples.clear();
    }

    _cells.Put(cells);
}

void GridSampleHolder::GetSamples(vector<const Sample *> & samples) const
{
    samples.clear();
    
    vector<Cell> cells;
    _cells.Get(cells);

    for(unsigned int i = 0; i < cells.size(); i++)
    {
        for(unsigned int j = 0; j < cells[i].samples.size(); j++)
        {
            samples.push_back(cells[i].samples[j]);
        }
    }
}

int GridSampleHolder::GetNeighbors(const Sample & query, const float radius, vector<const Sample *> & neighbors) const
{
    neighbors.clear();

    const int dimension = Dimension();

    vector<int> cell_index(dimension);
    if(! LocateCell(query, cell_index))
    {
        // outside the grid
        return 0;
    }

    const float radius_in_cells = (radius/_domain_spec.cell_size + 1);

    NBallSliceCounter counter(dimension, radius_in_cells*radius_in_cells);

    vector<int> offset_index(dimension);
    vector<int> current_index(dimension);
    vector<int> corrected_index(dimension);
    
    counter.Reset();
    do
    {
        counter.Get(offset_index);

        for(unsigned int i = 0; i < current_index.size(); i++)
        {
            current_index[i] = cell_index[i] + offset_index[i];

            if(_domain.Boundary() == Domain::BOUNDARY_TOROIDAL)
            {
                corrected_index[i] = (((current_index[i]%_cells.Size(i))+_cells.Size(i))%_cells.Size(i));
            }
            else
            {
                corrected_index[i] = current_index[i];
            }
        }

        Cell current_cell;
        if(_cells.Get(corrected_index, current_cell))
        {
            for(unsigned int j = 0; j < current_cell.samples.size(); j++)
            {
                neighbors.push_back(current_cell.samples[j]);
            }
        }
    }
    while(counter.Next());

    // done
    return 1;
}

int GridSampleHolder::GetSamples(const vector<int> & cell_index, vector<const Sample *> & samples) const
{
    samples.clear();

    Cell cell;

    if(!_cells.Get(cell_index, cell))
    {
        return 0;
    }
    else
    {
        for(unsigned int j = 0; j < cell.samples.size(); j++)
        {
            samples.push_back(cell.samples[j]);
        }

        return 1;
    }
}

int GridSampleHolder::LocateCell(const Sample & sample, vector<int> & cell_index) const
{
    if(! Inside(sample)) return 0;
    
    if(static_cast<unsigned int>(sample.coordinate.Dimension()) != cell_index.size())
    {
        cell_index = vector<int>(sample.coordinate.Dimension());
    }
    
    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        cell_index[i] = static_cast<int>(floor(sample.coordinate[i]/_domain_spec.cell_size));
    }
    
    return 1;
}

int GridSampleHolder::CellOffset(const float cell_size, const float min_distance)
{
    return static_cast<int>(ceil(min_distance/cell_size));
}
