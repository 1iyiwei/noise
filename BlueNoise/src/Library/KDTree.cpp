/*
  KDTree.cpp

  Li-Yi Wei
  07/11/2007

*/

#include <iostream>
using namespace std;

#include <assert.h>
#include <math.h>

#include "KDTree.hpp"
#include "SequentialCounter.hpp"
#include "Exception.hpp"

#define _USE_MEAN_NOT_MAX_METRIC

KDTree::Cell::Cell(KDTree & tree, const Cell * parent, const vector<int> & child_index) : _tree(tree), _parent(parent), _level(parent ? parent->_level+1 : 0), _index(child_index)
{
    if(_parent)
    {
        assert(_index.size() == parent->_index.size());
    
        for(unsigned int i = 0; i < _index.size(); i++)
        {
            _index[i] += 2*_parent->_index[i];
        }
    }
}

KDTree::Cell::~Cell(void)
{
    // nothing to do
    // the containing tree will be responsible for freeing memory
}

int KDTree::Cell::Level(void) const
{
    return _level;
}

float KDTree::Cell::Size(void) const
{
    return _tree.CellSize(_level);
}

int KDTree::Cell::GetIndex(vector<int> & answer) const
{
    answer = _index;
    return 1;
}

int KDTree::Cell::NumSamples(void) const
{
    return _samples.size();
}

int KDTree::Cell::NumChildren(void) const
{
    return _children.size();
}

const KDTree::Cell * KDTree::Cell::Parent(void) const
{
    return _parent;
}
        
int KDTree::Cell::GetSamples(vector<const Sample *> & samples) const
{
    samples = _samples;
    return 1;
}
        
int KDTree::Cell::GetChildren(vector<Cell *> & children)
{
    children = _children;
    return 1;
}

int KDTree::Cell::GetChildren(vector<const Cell *> & children) const
{
    if(children.size() != _children.size())
    {
        children = vector<const Cell *>(_children.size());
    }

    for(unsigned int i = 0; i < _children.size(); i++)
    {
        children[i] = _children[i];
    }

    return 1;
}

int KDTree::Cell::Subdivide(void)
{
    if(_samples.size() <= 0)
    {
        // no subdivide
        return 0;
    }
    else
    {
        const float cell_size = _tree.CellSize(_level);
        
        // check samples min distance
        for(unsigned int i = 0; i < _samples.size(); i++)
        {
            assert(_samples[i]);

#ifdef _USE_MEAN_NOT_MAX_METRIC
            const float sample_min_distance = _tree.GetDistanceField().Query(_samples[i]->coordinate)/2;
#else
            const float sample_min_distance = _tree.GetDistanceField().Query(_samples[i]->coordinate);
#endif
            
            if(cell_size*sqrt(_tree.Dimension()*1.0) <= sample_min_distance)
            {
                return 0;
            }
        }
    }

    // OK to proceed with subdivision
    if(_children.size() <= 0)
    {
        // spawn children
        SequentialCounter counter(_index.size(), 0, 1);
        counter.Reset();

        vector<int> child_index(_index.size());
        do
        {
            counter.Get(child_index);

            Cell * new_born = new Cell(_tree, this, child_index);

            if(new_born) _children.push_back(new_born);
            else return 0;
        }
        while(counter.Next());

        // migrate sample
        while(_samples.size() > 0)
        {
            int found_child = 0;
            for(unsigned int i = 0; (i < _children.size()) & !found_child; i++)
            {
                if(_children[i]->Inside(*_samples[0]))
                {
                    if(! _children[i]->Add(*_samples[0])) return 0;
                    found_child = 1;
                }
            }

            if(! found_child)
            {
                return 0;
            }
            else
            {
                _samples[0] = _samples[_samples.size()-1];
                _samples.pop_back();
            }
        }
        
        // done
        return 1;
    }
    else
    {
        // already subdivided
        assert(0);
        return 2;
    }
}

const KDTree::Cell * KDTree::Cell::FindCell(const int level, const vector<int> & index) const
{
    if((level == _level))
    {
        assert(index.size() == _index.size());
        int same = 1;
        for(unsigned int i = 0; i < index.size(); i++)
        {
            if(index[i] != _index[i]) same = 0;
        }

        if(same)
        {
            return this;
        }
        else
        {
            return 0;
        }
    }
    else if(level > _level)
    {
        // evil recursive calls
        const Cell * answer = 0;

        if(_children.size() > 0)
        {
            vector<int> potential_child_index = index;

            {
                for(int n = level; n > (_level+1); n--)
                {
                    for(unsigned int i = 0; i < potential_child_index.size(); i++)
                    {
                        potential_child_index[i] /= 2;
                    }
                }

                {
                    for(unsigned int i = 0; i < potential_child_index.size(); i++)
                    {
                        potential_child_index[i] -= 2*_index[i];
                        assert((potential_child_index[i] >= 0) && (potential_child_index[i] <= 1));
                    }
                }
            }
            
            // this part assumes the children are sorted in SequentialCounter order
            int which_child = 0;
            {
                assert(potential_child_index.size() == _tree.Dimension());
                
                for(int i = static_cast<int>(potential_child_index.size())-1; i >= 0; i--)
                {
                    which_child *= 2;
                    which_child += potential_child_index[i];
                }
            }
            
            assert((which_child >= 0) && (which_child < _children.size()));

            // evil recursive call, should expand it
            return _children[which_child]->FindCell(level, index);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
    
int KDTree::Cell::Inside(const Sample & sample) const
{
    const float cell_size = _tree.CellSize(_level);

    if(cell_size > 0)
    {
        if(_index.size() == sample.coordinate.Dimension())
        {
            for(unsigned int i = 0; i < _index.size(); i++)
            {
                const float dim_i_min = _index[i]*cell_size;

                if((sample.coordinate[i] < dim_i_min) ||
                   (sample.coordinate[i] > (dim_i_min+cell_size)))
                {
                    return 0;
                }
            }

            return 1;
        }
        else
        {
            // error
            return 0;
        }
    }
    else
    {
        // error
        return 0;
    }
}
        
int KDTree::Cell::EccentricConflict(const Sample & sample) const
{
    int foo;
    return EccentricConflict(sample, foo);
}

int KDTree::Cell::EccentricConflict(const Sample & sample, int & num_neighbors_checked) const
{
    const float sample_min_distance = _tree.GetDistanceField().Query(sample.coordinate);
    
    const Cell * container = this;

#if 0
    throw Exception("don't use this hypercube counter as it is freaking slow!");
    const int cell_offset = 2*static_cast<int>(ceil(sqrt(_tree.Dimension()*1.0)));
    
    SequentialCounter counter(_tree.Dimension(), -cell_offset, cell_offset);
#else
    Counter & counter = _tree.GetCounter();
#endif
    
    vector<int> sibling_offset(_tree.Dimension());
    vector<int> container_index(_tree.Dimension());

    vector<int> current_index(_tree.Dimension());
    vector<int> corrected_index(_tree.Dimension());
    
    vector<int> num_cells_per_dimension(_tree.Dimension());
    vector<const Sample *> samples(1);

    while(container) // stepping through all ancestors from young to old
    {
        _tree.GetNumCellsPerDimension(container->Level(), num_cells_per_dimension);

        container->GetIndex(container_index);
        
        counter.Reset();

        const float cell_size = _tree.CellSize(container->Level());

        num_neighbors_checked = 0;
        
        // loop through all siblings
        do
        {
            num_neighbors_checked++;
                    
            counter.Get(sibling_offset);

            int not_needed = 0;
            assert(sibling_offset.size() == container_index.size());
            {
                for(unsigned int i = 0; i < sibling_offset.size(); i++)
                {
#if 0
                    // optimization to reduce repeatition of neighbor checking
                    // assuming that the set of neighbors has no hole inside
                    if((sibling_offset[i] < -(num_cells_per_dimension[i]-1)) || (sibling_offset[i] > (num_cells_per_dimension[i]-1)))
                    {
                        not_needed = 1;
                    }
#endif
                    
                    current_index[i] = container_index[i]+sibling_offset[i];

                    const DistanceField::Boundary boundary_handling = _tree.GetDistanceField().BoundaryCondition(i);

                    // toroidal mode
                    corrected_index[i] = ((current_index[i]%num_cells_per_dimension[i])+num_cells_per_dimension[i])%num_cells_per_dimension[i];

                    if((boundary_handling == DistanceField::BOUNDARY_NONE) &&
                       (corrected_index[i] != current_index[i]))
                    {
                        not_needed = 1;
                    }
                }
            }

            const Cell * sibling = not_needed ? 0 : _tree.FindCell(container->Level(), corrected_index);
            
            if(sibling && sibling->GetSamples(samples))
            {
                for(unsigned int i = 0; i < samples.size(); i++)
                {
                    float distance = 0;
                    assert(samples[i]);

                    const Sample & contestant = *samples[i];
                    assert(sample.coordinate.Dimension() == contestant.coordinate.Dimension());

                    const float contestant_min_distance = _tree.GetDistanceField().Query(contestant.coordinate);
    
                    for(int j = 0; j < sample.coordinate.Dimension(); j++)
                    {
                        const float foo = sample.coordinate[j]-(contestant.coordinate[j]+(current_index[j]-corrected_index[j])*cell_size);
                        distance += foo*foo;
                    }

#ifdef _USE_MEAN_NOT_MAX_METRIC
                    // mean metric
                    distance = sqrt(distance);
                    if(distance < (0.5*(sample_min_distance + contestant_min_distance)))
                    {
                        return 1;
                    }                        
#else
                    // max metric
                    if((distance < sample_min_distance*sample_min_distance) ||
                       (distance < contestant_min_distance*contestant_min_distance))
                    {
                        return 1;
                    }
#endif
                }
            }
        }
        while(counter.Next());
        
        container = container->Parent();
    }
    
    // done, no conflict
    return 0;
}

int KDTree::Cell::Add(const Sample & sample)
{
    if(_samples.size() == 0)
    {
        _samples.push_back(&sample);
  
        return 1;
    }
    else
    {
        return 0;
    }
}

KDTree::KDTree(const int dimension, const DistanceField & distance_field) : _dimension(dimension), _distance_field(distance_field), _counter(dimension, ConflictRadiusSquare(dimension, distance_field.IsUniform()))
{
    BuildRootCells(1.0);
}

KDTree::KDTree(const int dimension, const float cell_size, const DistanceField & distance_field) : _dimension(dimension), _distance_field(distance_field), _counter(dimension, ConflictRadiusSquare(dimension, distance_field.IsUniform()))
{
    BuildRootCells(cell_size);
}

void KDTree::BuildRootCells(const float cell_size_input)
{
    const float cell_size = ((cell_size_input >= 0) && (cell_size_input <= 1)) ? cell_size_input : 1.0;

    _cells_per_level = vector< vector<Cell *> > (1);
    _cell_size_per_level = vector<float>(1);
    _num_cells_per_dimension_per_level = vector< vector<int> >(1);

    vector<float> range_min(_dimension);
    vector<float> range_max(_dimension);
    _distance_field.GetRange(range_min, range_max);

    vector<int> num_cells_per_dim_min(_dimension);
    vector<int> num_cells_per_dim_max(_dimension);
    {
        for(int i = 0; i < _dimension; i++)
        {
            num_cells_per_dim_min[i] = 0;
            num_cells_per_dim_max[i] = static_cast<int>(ceil((range_max[i]-range_min[i])/cell_size))-1;

            if(num_cells_per_dim_max[i] < num_cells_per_dim_min[i])
            {
                num_cells_per_dim_max[i] = num_cells_per_dim_min[i];
            }
            assert(num_cells_per_dim_min[i] <= num_cells_per_dim_max[i]);
        }
    }
    
    SequentialCounter counter(_dimension, num_cells_per_dim_min, num_cells_per_dim_max);

    counter.Reset();
    
    vector<int> index(_dimension);

    do
    {
        counter.Get(index);
        _cells_per_level[0].push_back(new Cell(*this, 0, index));
    }
    while(counter.Next());
    
    _cell_size_per_level[0] = cell_size;

    _num_cells_per_dimension_per_level[0] = vector<int>(_dimension);
    {
        for(unsigned int i = 0; i < _num_cells_per_dimension_per_level[0].size(); i++)
        {
            _num_cells_per_dimension_per_level[0][i] = num_cells_per_dim_max[i]+1;
        }
    }
}

KDTree::~KDTree(void)
{
    // release all cells

    for(unsigned int level = 0; level < _cells_per_level.size(); level++)
    {
        for(unsigned int i = 0; i < _cells_per_level[level].size(); i++)
        {
            if(_cells_per_level[level][i])
            {
                delete _cells_per_level[level][i];
                _cells_per_level[level][i] = 0;
            }
        }
    }

    _cells_per_level.clear();
    _cell_size_per_level.clear();
}

const DistanceField & KDTree::GetDistanceField(void) const
{
    return _distance_field;
}

Counter & KDTree::GetCounter(void)
{
    return _counter;
}

int KDTree::Dimension(void) const
{
    return _dimension;
}

int KDTree::NumLevels(void) const
{
    assert(_cells_per_level.size() == _cell_size_per_level.size());
    return _cells_per_level.size();
}

float KDTree::CellSize(const int level) const
{
    if((level >= 0) && (level < NumLevels()))
    {
        return _cell_size_per_level[level];
    }
    else
    {
        return -1;
    }
}

int KDTree::NumCells(const int level) const
{
    if((level >= 0) && (level < NumLevels()))
    {
        return _cells_per_level[level].size();
    }
    else
    {
        return -1;
    }
}

float KDTree::ConflictRadiusSquare(void) const
{
    return ConflictRadiusSquare(_dimension, _distance_field.IsUniform());
}

float KDTree::ConflictRadiusSquare(const int dimension, const int uniform_distance_field)
{
    return (uniform_distance_field ? 4*dimension : 9*dimension);
}

int KDTree::GetNumCellsPerDimension(const int level, vector<int> & answer) const
{
    if((level >= 0) && (static_cast<unsigned int>(level) < _num_cells_per_dimension_per_level.size()))
    {
        answer = _num_cells_per_dimension_per_level[level];
        return 1;
    }
    else
    {
        return 0;
    }
}
    
int KDTree::GetCells(const int level, vector<Cell *> & cells)
{
    if((level >= 0) && (level < NumLevels()))
    {
        cells = _cells_per_level[level];
        return 1;
    }
    else
    {
        return 0;
    }
}

const KDTree::Cell * KDTree::FindCell(const int level, const vector<int> & index) const
{
    const KDTree::Cell * answer = 0;

    // find the root node that potentially contains the cell
    vector<int> root_index = index;

    for(int n = level; n > 0; n--)
    {
        for(unsigned int i = 0; i < root_index.size(); i++)
        {
            root_index[i] /= 2;
        }
    }

    const vector<int> & num_root_cells_per_dim = _num_cells_per_dimension_per_level[0];

    {
        assert(root_index.size() == num_root_cells_per_dim.size());
        for(unsigned int i = 0; i < root_index.size(); i++)
        {
            assert((root_index[i] >= 0) && (root_index[i] < num_root_cells_per_dim[i]));
        }
    }
    
    // assuming root nodes sorted in sequential order
    // make sure this is consistent with the constructor

    int which_root = 0;
    {
        for(int i = num_root_cells_per_dim.size()-1; i >= 0; i--)
        {
            which_root *= num_root_cells_per_dim[i];
            which_root += root_index[i];
        }
    }

    assert((which_root >= 0) && (which_root < _cells_per_level[0].size()));
    
    return _cells_per_level[0][which_root]->FindCell(level, index);
}

int KDTree::GetCells(const int level, vector<const Cell *> & cells) const
{
    if((level >= 0) && (level < NumLevels()))
    {
        if(cells.size() != _cells_per_level[level].size())
        {
            cells = vector<const Cell *>(_cells_per_level[level].size());
        }

        for(unsigned int i = 0; i < cells.size(); i++)
        {
            cells[i] = _cells_per_level[level][i];
        }
        
        return 1;
    }
    else
    {
        return 0;
    }
}

void KDTree::GetSamples(vector<const Sample *> & samples) const
{
    samples.clear();

    vector<const Sample *> cell_samples;
    
    for(unsigned int i = 0; i < _cells_per_level.size(); i++)
    {
        for(unsigned int j = 0; j < _cells_per_level[i].size(); j++)
        {
            if(_cells_per_level[i][j] && _cells_per_level[i][j]->GetSamples(cell_samples))
            {
                for(unsigned int k = 0; k < cell_samples.size(); k++)
                {
                    samples.push_back(cell_samples[k]);
                }
            }
        }
    }
}
    
int KDTree::Subdivide(void)
{
    assert(_cells_per_level.size() == _cell_size_per_level.size());
    assert(_cell_size_per_level.size() == _num_cells_per_dimension_per_level.size());

    assert(_cells_per_level.size() > 0);
    
    const int current_level = _cells_per_level.size()-1;

    int any_subdivided = 0;

    vector<Cell *> empty; empty.clear();
    _cells_per_level.push_back(empty);
    
    _cell_size_per_level.push_back(_cell_size_per_level[_cell_size_per_level.size()-1]/2.0);
    
    _num_cells_per_dimension_per_level.push_back(_num_cells_per_dimension_per_level[_num_cells_per_dimension_per_level.size()-1]);
    
    {
        for(unsigned int i = 0; i < _num_cells_per_dimension_per_level[_num_cells_per_dimension_per_level.size()-1].size(); i++)
        {
            _num_cells_per_dimension_per_level[_num_cells_per_dimension_per_level.size()-1][i] *= 2;
        }
    }

    // go through all nodes at bottom level and try to subdivide them
    {
        vector<Cell *> children;
        
        for(unsigned int i = 0; i < _cells_per_level[current_level].size(); i++)
        {
            assert(_cells_per_level[current_level][i]);
            
            if(_cells_per_level[current_level][i]->Subdivide())
            {
                any_subdivided = 1;
            }

            if(_cells_per_level[current_level][i]->GetChildren(children))
            {
                for(unsigned int j = 0; j < children.size(); j++)
                {
                    _cells_per_level[_cells_per_level.size()-1].push_back(children[j]);
                }
            }
        }
    }

    return any_subdivided;
}
