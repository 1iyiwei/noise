/*
  Grid.cpp

  Li-Yi Wei
  04/26/2008

*/

#include "Grid.hpp"
#include "Exception.hpp"
#include "SequentialCounter.hpp"
#include "Math.hpp"

#include <math.h>

#define _ALLOW_MULTIPLE_SAMPLES_PER_CELL

Grid::DomainSpec Grid::BuildDomainSpec(const vector<float> & domain_spec)
{
    const int dimension = domain_spec.size();

    Grid::DomainSpec grid_domain_spec;

    // default/initial value
    grid_domain_spec.domain_size = vector<int>(dimension, 1);
    grid_domain_spec.cell_size = 1.0;

    // start computation
    vector<float> domain_size(domain_spec);
    float cell_size = 1.0;

    while(!Math::AllInteger(domain_size))
    {
        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            domain_size[i] *= 10.0;
        }
        cell_size /= 10.0;
    }

    for(unsigned int i = 0; i < grid_domain_spec.domain_size.size(); i++)
    {
        grid_domain_spec.domain_size[i] = static_cast<int>(domain_size[i]);
    }
    grid_domain_spec.cell_size = cell_size;

    const int gcd = Math::GreatestCommonDivisor(grid_domain_spec.domain_size);
    for(unsigned int i = 0; i < grid_domain_spec.domain_size.size(); i++)
    {
        grid_domain_spec.domain_size[i] /= gcd;
    }
    grid_domain_spec.cell_size *= gcd;

    // done
    return grid_domain_spec;
}

Grid::Cell::Cell(void)
{
    // nothing to do
}

Grid::Grid(const DomainSpec & spec, const float min_distance) : _domain_spec(ComputeDomainSpec(spec, min_distance)), _cells(_domain_spec.domain_size)
{
    // nothing to do
}

Grid::~Grid(void)
{
    // nothing to do
}

int Grid::Dimension(void) const
{
    return _domain_spec.domain_size.size();
}

vector<int> Grid::NumCells(void) const
{
    return _domain_spec.domain_size;
}

float Grid::CellSize(void) const
{
    return _domain_spec.cell_size;
}

int Grid::Inside(const Sample & sample) const
{
    if(sample.coordinate.Dimension() != Dimension())
    {
        throw Exception("Grid::Inside(): sample dimension mismatch");
    }

    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        if((sample.coordinate[i] <= 0) || (sample.coordinate[i] >= (_domain_spec.domain_size[i]*_domain_spec.cell_size)))
        {
            return 0;
        }
    }

    return 1;
}

int Grid::Conflict(const Sample & sample, const ConflictChecker & checker) const
{
    const float worst_case_distance = checker.MaxDistance();

    vector<const Sample *> neighbors;

    const int status = GetConflicts(sample, checker, neighbors, 0);

    if(! status) return 0;

    return (neighbors.size() > 0);
}

int Grid::Add(const Sample & sample)
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

int Grid::Remove(const Sample & sample)
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

void Grid::GetSamples(vector<const Sample *> & samples) const
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

int Grid::GetConflicts(const Sample & sample, const ConflictChecker & checker, vector<const Sample *> & neighbors) const
{
    return GetConflicts(sample, checker, neighbors, 1);
}

int Grid::GetConflicts(const Sample & sample, const ConflictChecker & checker, vector<const Sample *> & neighbors, const int find_all) const
{
    const float worst_case_distance = checker.MaxDistance();

    neighbors.clear();

    if(worst_case_distance <= 0)
    {
        return 0;
    }
    
    const int dimension = Dimension();

    vector<int> cell_index(dimension);
    if(! LocateCell(sample, cell_index))
    {
        // outside the grid
        return 0;
    }

    const int cell_offset = CellOffset(_domain_spec.cell_size, worst_case_distance);
    
    vector<int> current_index(dimension);
    vector<int> corrected_index(dimension);

    vector<int> offset_index(dimension);

    SequentialCounter offset_index_counter(dimension, -cell_offset, cell_offset);

    offset_index_counter.Reset();
                
    vector<int> num_cells_per_dim(dimension);
    _cells.Size(num_cells_per_dim);

    do
    {
        offset_index_counter.Get(offset_index);
        assert(offset_index.size() == dimension);

        // test current offset_index
        for(unsigned int i = 0; i < current_index.size(); i++)
        {
            current_index[i] = cell_index[i] + offset_index[i];
            corrected_index[i] = (((current_index[i]%num_cells_per_dim[i])+num_cells_per_dim[i])%num_cells_per_dim[i]);
        }
        
        Cell current_cell;
        if(!_cells.Get(corrected_index, current_cell))
        {
            throw Exception("!_cells.Get(corrected_index, current_cell)");
        }

        
        for(unsigned int j = 0; j < current_cell.samples.size(); j++)
        {
            Sample current_sample = *current_cell.samples[j];

            for(int i = 0; i < dimension; i++)
            {
                // toroidal boundary handling
                current_sample.coordinate[i] += ((current_index[i]-corrected_index[i])*_domain_spec.cell_size);
            }

            const ConflictChecker::Status status = checker.Check(sample, current_sample);

            if(status == ConflictChecker::CHECK_ERROR) throw Exception("error for conflict check");

            if(status == ConflictChecker::CHECK_FAIL)
            {
                neighbors.push_back(current_cell.samples[j]);
                if(! find_all) return 1;
            }
        }
    }
    while(offset_index_counter.Next());

    // done
    return 1;
}

Grid::DomainSpec Grid::ComputeDomainSpec(const DomainSpec & input, const float min_distance)
{
    if(input.cell_size <= 0)
    {
        throw Exception("cell_size <= 0");
    }

    const int amplification = static_cast<int>(ceil(input.cell_size/min_distance*sqrt(input.domain_size.size()*1.0)));

    if(amplification < 1)
    {
        throw Exception("amplification < 1");
    }

    DomainSpec answer = input;

    answer.cell_size /= amplification;

    for(unsigned int i = 0; i < answer.domain_size.size(); i++)
    {
        answer.domain_size[i] *= amplification;
    }

    return answer;
}

int Grid::LocateCell(const Sample & sample, vector<int> & cell_index) const
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

int Grid::CellOffset(const float cell_size, const float min_distance)
{
    return static_cast<int>(ceil(min_distance/cell_size));
}
