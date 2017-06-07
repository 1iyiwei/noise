/*
  Grid.cpp

  Li-Yi Wei
  06/28/2007

*/

#include <math.h>
#include "Grid.hpp"
#include "SequentialCounter.hpp"

Grid::Cell::Cell(void) : sample(0)
{
    // nothing to do
}

int Grid::CellOffset(const int dimension)
{
    return static_cast<int>(ceil(sqrt(dimension*1.0)));
}
    
int Grid::CliqueSize(const int dimension)
{
    return CellOffset(dimension)+1;
}

int Grid::CellOffset(const int dimension, const float cell_size, const float min_distance)
{
    return static_cast<int>(ceil(min_distance/cell_size));
}

int Grid::CliqueSize(const int dimension, const float cell_size, const float min_distance)
{
    return CellOffset(dimension, cell_size, min_distance)+1;
}

Grid::Grid(const int dimension, const float min_distance, const int clique_size) : _dim(dimension), _num_cells_per_dim(P2(((static_cast<int>(ceil(sqrt(dimension*1.0)/min_distance)) + clique_size-1)/clique_size)*clique_size)), _cell_size(1.0/_num_cells_per_dim)
{
    int total_num_cells = 1;
    
    for(int i = 0; i < dimension; i++)
    {
        total_num_cells *= _num_cells_per_dim;
    }
    
    _cells = vector<Cell>(total_num_cells);
}

int Grid::Inside(const Sample & sample) const
{
    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        if((sample.coordinate[i] <= 0) || (sample.coordinate[i] >= 1.0))
        {
            return 0;
        }
    }
    
    return 1;
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
        cell_index[i] = static_cast<int>(floor(sample.coordinate[i]/_cell_size));
    }
    
    return 1;
}
    
Grid::Cell * Grid::GetCell(const vector<int> & index)
{
    const int internal_index = ConvertCellIndex(index);
    
    if((internal_index >= 0) && (internal_index < _cells.size()))
    {
        return &_cells[internal_index];
    }
    else
    {
        return 0;
    }
}
    
const Grid::Cell * Grid::GetCell(const vector<int> & index) const
{
    const int internal_index = ConvertCellIndex(index);
    
    if((internal_index >= 0) && (internal_index < _cells.size()))
    {
        return &_cells[internal_index];
    }
    else
    {
        return 0;
    }
}
    
int Grid::Conflict(const Sample & sample, const float min_distance) const
{
    if(min_distance <= 0)
    {
        return 0;
    }
    
    vector<int> cell_index(_dim);
    if(! LocateCell(sample, cell_index))
    {
        // outside the grid
        return 0;
    }

    const int cell_offset = CellOffset(_dim, _cell_size, min_distance);
    
    vector<int> current_index(_dim);
    vector<int> corrected_index(_dim);

    vector<int> offset_index(_dim);

    SequentialCounter offset_index_counter(_dim, -cell_offset, cell_offset);

    offset_index_counter.Reset();
                
    do
    {
        offset_index_counter.Get(offset_index);
        assert(offset_index.size() == _dim);

        // test current offset_index
        for(unsigned int i = 0; i < current_index.size(); i++)
        {
            current_index[i] = cell_index[i] + offset_index[i];
            corrected_index[i] = (((current_index[i]%_num_cells_per_dim)+_num_cells_per_dim)%_num_cells_per_dim);
        }
        
        const Cell * current_cell = GetCell(corrected_index);
        
        assert(current_cell);
                
        if(current_cell && current_cell->sample)
        {
            float distance = 0;
            for(int i = 0; i < _dim; i++)
            {
                // the third term is for toroidal boundary handling
                const float foo = sample.coordinate[i]-(current_cell->sample->coordinate[i]+(current_index[i]-corrected_index[i])*_cell_size);
                distance += foo*foo;
            }

            if(distance < min_distance*min_distance)
            {
                return 1;
            }
        }
    }
    while(offset_index_counter.Next());

    // done, no conflict
    return 0;
}

int Grid::Add(const Sample & sample)
{
    vector<int> cell_index(_dim);
    
    if(! LocateCell(sample, cell_index))
    {
        return 0;
    }
            
    Cell * cell = GetCell(cell_index);
    
    if(!cell || (cell->sample))
    {
        return 0;
    }
    else
    {
        cell->sample = &sample;
        return 1;
    }
}

void Grid::GetSamples(vector<const Sample *> & samples) const
{
    samples.clear();
    
    for(unsigned int i = 0; i < _cells.size(); i++)
    {
        if(_cells[i].sample)
        {
            samples.push_back(_cells[i].sample);
        }
    }
}

int Grid::Dimension(void) const
{
    return _dim;
}
    
int Grid::NumCellsPerDimension(void) const
{
    return _num_cells_per_dim;
}
    
float Grid::CellSize(void) const
{
    return _cell_size;
}

int Grid::CellOffset(const float min_distance) const
{
    return CellOffset(_dim, _cell_size, min_distance);
}

int Grid::CliqueSize(const float min_distance) const
{
    return CellOffset(min_distance)+1;
}
    
int Grid::ConvertCellIndex(const vector<int> & index) const
{
    if(static_cast<unsigned int>(_dim) != index.size())
    {
        return -1;
    }
    
    int result = 0;
    
    for(int i = 0; i < _dim; i++)
    {
        result *= _num_cells_per_dim; //(i >= 1? index[i-1] : 1);
        result += index[i];
        
        if((index[i] < 0) || (index[i] >= _num_cells_per_dim))
        {
            return -1;
        }
    }
            
    return result;
}       
 
int Grid::P2(const int value)
{
    int answer = 1;

    if(value > 0)
    {
        while(answer < value) answer *= 2;
    }

    return answer;
}
