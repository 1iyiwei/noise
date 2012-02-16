/*
  Tree.cpp

  Li-Yi Wei
  07/09/2008

*/

#include <math.h>

#include "Tree.hpp"
#include "SequentialCounter.hpp"
#include "Exception.hpp"

Tree::Tree(const DistanceField & distance_field, const ConflictChecker & conflict_checker) : _distance_field(distance_field), _conflict_checker(conflict_checker), _cell_offset(CellOffset(distance_field.Dimension(), conflict_checker)), _conflict_metric_from_checker(1), _pools(distance_field.NumClasses())
{
    BuildRootCells(Grid::BuildDomainSpec(distance_field.Domain()));

    if(!InitPoolsWithEmptyLeaves())
    {
        throw Exception("cannot InitPoolsWithEmptyLeaves()");
    }
}

Tree::Tree(const DistanceField & distance_field, const ConflictChecker & conflict_checker, const DomainSpec & spec) : _distance_field(distance_field), _conflict_checker(conflict_checker), _cell_offset(CellOffset(distance_field.Dimension(), conflict_checker)), _conflict_metric_from_checker(0), _pools(distance_field.NumClasses())
{
    BuildRootCells(spec);

    if(!InitPoolsWithEmptyLeaves())
    {
        throw Exception("cannot InitPoolsWithEmptyLeaves()");
    }
}

void Tree::BuildRootCells(const DomainSpec & spec)
{
    const int dimension = spec.domain_size.size();
    const float cell_size = spec.cell_size;

    _cells_per_level = vector< vector<Cell *> > (1);
    _cell_size_per_level = vector<float>(1);
    _num_cells_per_dimension_per_level = vector< vector<int> >(1);

    vector<int> num_cells_per_dim_min(dimension);
    vector<int> num_cells_per_dim_max(dimension);
    {
        for(int i = 0; i < dimension; i++)
        {
            num_cells_per_dim_min[i] = 0;
            num_cells_per_dim_max[i] = spec.domain_size[i]-1;

            if(num_cells_per_dim_max[i] < num_cells_per_dim_min[i])
            {
                num_cells_per_dim_max[i] = num_cells_per_dim_min[i];
            }
            assert(num_cells_per_dim_min[i] <= num_cells_per_dim_max[i]);
        }
    }
    
    SequentialCounter counter(dimension, num_cells_per_dim_min, num_cells_per_dim_max);

    counter.Reset();
    
    vector<int> index(dimension);

    do
    {
        counter.Get(index);
        _cells_per_level[0].push_back(new Cell(*this, 0, index));
    }
    while(counter.Next());
    
    _cell_size_per_level[0] = cell_size;

    _num_cells_per_dimension_per_level[0] = vector<int>(dimension);
    {
        for(unsigned int i = 0; i < _num_cells_per_dimension_per_level[0].size(); i++)
        {
            _num_cells_per_dimension_per_level[0][i] = num_cells_per_dim_max[i]+1;
        }
    }
}

Tree::~Tree(void)
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
    _num_cells_per_dimension_per_level.clear();
}

const DistanceField & Tree::GetDistanceField(void) const
{
    return _distance_field;
}

const ConflictChecker & Tree::GetConflictChecker(void) const
{
    return _conflict_checker;
}

int Tree::Dimension(void) const
{
    return _distance_field.Dimension();
}

int Tree::NumLevels(void) const
{
    return _cells_per_level.size();
}

float Tree::CellSize(const int level) const
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

int Tree::NumCells(const int level) const
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

int Tree::GetNumCellsPerDimension(const int level, vector<int> & answer) const
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

int Tree::Conflict(const Sample & sample) const
{
    return Conflict(sample, 1);
}

int Tree::Conflict(const Sample & sample, const int same_tree) const
{
    int level = 0;

    const Cell * cell = LocateCell(sample, level);

    if(! cell)
    {
        throw Exception("Tree::Conflict(): cannot locate cell containing sample");
        return 0;
    }
    else
    {
        return cell->Conflict(sample, same_tree);
    }
}

int Tree::Add(const Sample & sample)
{
    int level = 0;

    Cell * cell = LocateCell(sample, level);

    if(! cell) return 0;

    return cell->Add(sample);
}

int Tree::Remove(const Sample & sample)
{
    int level = 0;

    Cell * cell = LocateCell(sample, level);

    if(! cell) return 0;

    return cell->Remove(sample);
}
 
int Tree::AddDeadZone(const Sample & sample)
{
    int level = 0;

    Cell * cell = LocateCell(sample, level);

    if(! cell) return 0;

    return cell->AddDeadZone(sample);
}

int Tree::RemoveDeadZone(const Sample & sample)
{
    int level = 0;

    Cell * cell = LocateCell(sample, level);

    if(! cell) return 0;

    return cell->RemoveDeadZone(sample);
}

int Tree::GetVisitCount(const int class_id) const
{
    if((class_id >= 0) && (class_id < _pools.size()))
    {
        return _pools[class_id].GetVisitCount();
    }
    else
    {
        throw Exception("Tree::GetVisitCount() : illegal class_id");
        return -1;
    }
}

int Tree::GetCells(const int level, vector<Cell *> & cells)
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

int Tree::GetCells(const int level, vector<const Cell *> & cells) const
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

int Tree::GetPossibleCells(const int class_id, vector<Cell *> & cells)
{
    vector<const Cell *> const_cells;

    if((class_id >= 0) && (class_id < _pools.size()))
    {
        if(_pools[class_id].GetCells(const_cells))
        {
            cells.clear();

            for(unsigned int i = 0; i < const_cells.size(); i++)
            {
                cells.push_back(const_cast<Cell *>(const_cells[i]));
            }

            return 1;
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

int Tree::GetPossibleCells(const int class_id, vector<const Cell *> & cells) const
{
    if((class_id >= 0) && (class_id < _pools.size()))
    {
        return _pools[class_id].GetCells(cells);
    }
    else
    {
        return 0;
    }
}

int Tree::NumPossibleCells(const int class_id) const
{
    if((class_id >= 0) && (class_id < _pools.size()))
    {
        return _pools[class_id].NumCells();
    }
    else
    {
        return 0;
    }
}

const Tree::Cell * Tree::GetPossibleCell(const int class_id, const int which_one) const
{
    if((class_id >= 0) && (class_id < _pools.size()))
    {
        return _pools[class_id].GetCell(which_one);
    }
    else
    {
        return 0;
    }
}

int Tree::NumLeafCells(void) const
{
    if(_cells_per_level.size() <= 0)
    {
        return 0;
    }
    else
    {
        return _cells_per_level[_cells_per_level.size()-1].size();
    }
}

const Tree::Cell * Tree::GetLeafCell(const int which_one) const
{
    const int num_leaf_cells = NumLeafCells();
    if((which_one >= 0) && (which_one < num_leaf_cells))
    {
        return _cells_per_level[_cells_per_level.size()-1][which_one];
    }
    else
    {
        return 0;
    }
}

Tree::Cell * Tree::GetLeafCell(const int which_one)
{
    const int num_leaf_cells = NumLeafCells();
    if((which_one >= 0) && (which_one < num_leaf_cells))
    {
        return _cells_per_level[_cells_per_level.size()-1][which_one];
    }
    else
    {
        return 0;
    }
}

const Tree::Cell * Tree::FindCell(const int level, const vector<int> & index) const
{
    const Cell * answer = 0;

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
        
int Tree::LocateCell(const Sample & sample, int & level, vector<int> & cell_index) const
{
    if(! _distance_field.Inside(sample))
    {
        return 0;
    }

    if(sample.coordinate.Dimension() != Dimension())
    {
        return 0;
    }

    const float cell_size = _cell_size_per_level[0];
    vector<int> root_index(Dimension(), 0);
    for(unsigned int i = 0; i < root_index.size(); i++)
    {
        root_index[i] = floor(sample.coordinate[i]/cell_size);
    }

    const vector<int> & num_root_cells_per_dim = _num_cells_per_dimension_per_level[0];

    {
        assert(root_index.size() == num_root_cells_per_dim.size());
        for(unsigned int i = 0; i < root_index.size(); i++)
        {
            if(!((root_index[i] >= 0) && (root_index[i] < num_root_cells_per_dim[i])))
            {
                throw Exception("Tree::LocateCell() : illegal root_index");
            }
        }
    }

    const Cell * cell = FindCell(0, root_index);

    if(! cell)
    {
        return 0;
    }
    else
    {
        return cell->LocateCell(sample, level, cell_index);
    }
}

const Tree::Cell * Tree::LocateCell(const Sample & sample, int & level) const
{
    vector<int> cell_index(sample.coordinate.Dimension());

    if(! LocateCell(sample, level, cell_index)) return 0;

    const Cell * cell = FindCell(level, cell_index);

    return cell;
}

Tree::Cell * Tree::LocateCell(const Sample & sample, int & level)
{
    vector<int> cell_index(sample.coordinate.Dimension());

    if(! LocateCell(sample, level, cell_index)) return 0;

    Cell * cell = const_cast<Cell *>(FindCell(level, cell_index));

    return cell;
}

void Tree::GetSamples(vector<const Sample *> & samples) const
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

int Tree::GetConflicts(const Sample & sample, vector<const Sample *> & neighbors) const
{
    return GetConflicts(sample, 1, neighbors);
}

int Tree::GetConflicts(const Sample & sample, const int same_tree, vector<const Sample *> & neighbors) const
{
    int level = 0;

    const Cell * cell = LocateCell(sample, level);

    if(! cell) return 0;

    return cell->GetConflicts(sample, same_tree, neighbors);
}

int Tree::Subdivide(void)
{
    return Subdivide(0); // non-blind
}

int Tree::Subdivide(const int blind)
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
            
            if(_cells_per_level[current_level][i]->Subdivide(blind))
            {
                any_subdivided = 1;
            
                if(_cells_per_level[current_level][i]->GetChildren(children))
                {
                    for(unsigned int j = 0; j < children.size(); j++)
                    {
                        _cells_per_level[_cells_per_level.size()-1].push_back(children[j]);
                    }
                }
            }
        }
    }

    if(any_subdivided)
    {
        // init pools
        if(! InitPoolsWithEmptyLeaves())
        {
            throw Exception("cannot InitPoolsWithEmptyLeaves()");
        }

        // update the conflict counter
        { 
            for(unsigned int i = 0; i < _cells_per_level[current_level].size(); i++)
            {
                if(! _cells_per_level[current_level][i]->Inherit())
                {
                    throw Exception("Tree::Subdivide(): error in Inherit()");
                }
            }
        }
    }
    else
    {
        if(_cells_per_level[_cells_per_level.size()-1].size() <= 0)
        {
            _cells_per_level.pop_back();
        }
        else
        {
            throw Exception("leaf level should be empty");
        }
    }

    return any_subdivided;
}

int Tree::ConflictMetricFromChecker(void) const
{
    return _conflict_metric_from_checker;
}

int Tree::CellOffset(void) const
{
    return _cell_offset;
}

int Tree::CellOffset(const int dimension, const ConflictChecker & conflict_checker)
{
    if(conflict_checker.ConflictMetric() == ConflictChecker::MEAN)
    {
        return static_cast<int>(ceil(5*sqrt(dimension*1.0)));
    }
    else
    {
        return static_cast<int>(ceil(3*sqrt(dimension*1.0)));
    }
}

int Tree::CellOffset(const float min_distance, const float cell_size)
{
    return static_cast<int>(ceil(min_distance/cell_size));
}

int Tree::InitPoolsWithEmptyLeaves(void)
{
    for(unsigned int j = 0; j < _pools.size(); j++)
    {
        _pools[j].Clear();

        if(_cells_per_level.size() > 0)
        {
            vector<Cell *> & leaves = _cells_per_level[_cells_per_level.size()-1];
            for(unsigned int i = 0; i < leaves.size(); i++)
            {
                if(!leaves[i]->PoolAdd(j))
                {
                    return 0;
                }
            }
        }
    }

    return 1;
}

Tree::Cell::Cell(Tree & tree, const Cell * parent, const vector<int> & child_index) : _tree(tree), _parent(parent), _level(parent ? parent->_level+1 : 0), _index(child_index), _pool_links(tree.GetDistanceField().NumClasses(), Pool::CellLink(this)), _visit_count(0)
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

int Tree::PoolAdd(const int class_id, Pool::CellLink & link)
{
    if((class_id >= 0) && (class_id < _pools.size()))
    {
        return _pools[class_id].Add(link);
    }
    else
    {
        return 0;
    }
}

int Tree::PoolRemove(const int class_id, Pool::CellLink & link)
{
    if((class_id >= 0) && (class_id < _pools.size()))
    {
        return _pools[class_id].Remove(link);
    }
    else
    {
        return 0;
    }
}

int Tree::AddVisitCount(const int class_id, const Pool::CellLink & link)
{
    if((class_id >= 0) && (class_id < _pools.size()))
    {
        return _pools[class_id].AddVisitCount(link);
    }
    else
    {
        return 0;
    }
}

Tree::Cell::~Cell(void)
{
    // nothing to do
}

int Tree::Cell::Level(void) const
{
    return _level;
}

float Tree::Cell::Size(void) const
{
    return _tree.CellSize(_level);
}

const vector<int> & Tree::Cell::Index(void) const
{
    return _index;
}

void Tree::Cell::GetDomain(vector<float> & min_corner, vector<float> & max_corner) const
{
    if(min_corner.size() != _index.size()) min_corner = vector<float>(_index.size());
    if(max_corner.size() != _index.size()) max_corner = vector<float>(_index.size());
    
    for(unsigned int i = 0; i < _index.size(); i++)
    {
        min_corner[i] = _index[i]*Size();
        max_corner[i] = min_corner[i] + Size();
    }
}

int Tree::Cell::NumSamples(void) const
{
    return _samples.size();
}

int Tree::Cell::NumChildren(void) const 
{
    return _children.size();
}

const Tree::Cell * Tree::Cell::Parent(void) const
{
    return _parent;
}
        
const Tree::Cell * Tree::Cell::FindCell(const int level, const vector<int> & index) const
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
    
int Tree::Cell::LocateCell(const Sample & sample, int & level, vector<int> & cell_index) const
{
    if(! Inside(sample))
    {
        return 0;
    }
    else
    {
        if(NumChildren() <= 0)
        {
            level = _level;
            cell_index = _index;
            return 1;
        }
        else
        {
            vector<int> child_index = _index;
            const float child_size = _tree.CellSize(_level+1);

            for(unsigned int i = 0; i < child_index.size(); i++)
            {
                child_index[i] = floor(sample.coordinate[i]/child_size);
            }

            const Cell * child = FindCell(_level+1, child_index);

            if(child)
            {
                return child->LocateCell(sample, level, cell_index);
            }
            else
            {
                throw Exception("weird situation in Tree::Cell::LocateCell()");
                return 0;
            }
        }
    }
}

int Tree::Cell::Inside(const Sample & sample) const
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
                   (sample.coordinate[i] >= (dim_i_min+cell_size)))
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

int Tree::Cell::CellOffset(void) const
{
    const int cell_offset = (_tree.ConflictMetricFromChecker() ? _tree.CellOffset() : _tree.CellOffset(_tree.GetConflictChecker().MaxDistance(), Size()));

    return cell_offset;
}

int Tree::Cell::Conflict(const Sample & sample) const
{
    return Conflict(sample, 1);
}

int Tree::Cell::Conflict(const Sample & sample, const int same_tree) const
{
    vector<const Sample *> neighbors;

    const int status = GetConflicts(sample, same_tree, neighbors, 0);

    if(! status)
    {
        throw Exception("Tree::Cell::Conflict(): error in GetConflicts");
        return 0;
    }

    return (neighbors.size() > 0);
}

int Tree::Cell::Add(const Sample & sample)
{
    if(_children.size() > 0)
    {
        // cannot add to an internal node
        throw Exception("cannot add to an internal node");
        return 0;
    }
    else if(_samples.size() == 0)
    {
        _samples.push_back(&sample);
  
        return 1;
    }
    else
    {
        throw Exception("cell already has sample");
        return 0;
    }
}

int Tree::Cell::Remove(const Sample & sample)
{
    int index = -1;

    for(unsigned int i = 0; i < _samples.size(); i++)
    {
        if(_samples[i] == &sample)
        {
            index = i;
        }
    }

    if(index >= 0)
    {
        _samples[index] = _samples[_samples.size()-1];
        _samples.pop_back();

        return 1;
    }
    else
    {
        // not found
        return 0;
    }
}

int Tree::Cell::AddDeadZone(const Sample & sample)
{
    // update conflict counter
    return UpdateDeadZone(sample, +1);
}

int Tree::Cell::RemoveDeadZone(const Sample & sample)
{
    // update conflict counter
    return UpdateDeadZone(sample, -1);
}

int Tree::Cell::Impossible(const int class_id) const
{
    return (_conflict_counter.Value(class_id) > 0);
}

int Tree::Cell::FarCorner(const Sample & sample, Coordinate & corner) const
{
    corner = sample.coordinate;

    for(int i = 0; i < corner.Dimension(); i++)
    {
        const float option0 = _index[i]*Size();
        const float option1 = (_index[i]+1)*Size();

        corner[i] = (fabs(sample.coordinate[i] - option0) > fabs(sample.coordinate[i] - option1) ? option0 : option1);
    }

    return 1;
}

int Tree::Cell::DeadZone(const Sample & sample, vector<int> verdict_per_class) const
{
    Sample corner(sample);

    if(! FarCorner(sample, corner.coordinate)) return 0;

    const ConflictChecker & checker = _tree.GetConflictChecker();

    for(unsigned int i = 0; i < verdict_per_class.size(); i++)
    {
        corner.id = i;

        const ConflictChecker::Status status = checker.Check(sample, corner);

        if(status == ConflictChecker::CHECK_ERROR) throw Exception("error for conflict check");
        
        verdict_per_class[i] = (status == ConflictChecker::CHECK_FAIL);
    }

    return 1;
}
 
int Tree::Cell::UpdateDeadZone(const Sample & sample, const int add_or_remove)
{
    const int cell_offset = CellOffset();

    SequentialCounter counter(_tree.Dimension(), -cell_offset, cell_offset);

    vector<int> num_cells_per_dim;
    if(! _tree.GetNumCellsPerDimension(Level(), num_cells_per_dim))
    {
        throw Exception("cannot get num cells per dimension");
        return 0;
    }

    const int dimension = _tree.Dimension();
    
    const vector<int> container_index = _index;
    vector<int> current_index(dimension);
    vector<int> corrected_index(dimension);
    vector<int> offset_index(dimension);

    Sample current_sample(sample);

    vector<int> verdict_per_class(_tree.GetDistanceField().NumClasses());

    counter.Reset();
    do
    {
        counter.Get(offset_index);
        
        // test current offset_index
        for(unsigned int i = 0; i < current_index.size(); i++)
        {
            current_index[i] = container_index[i] + offset_index[i];
            corrected_index[i] = (((current_index[i]%num_cells_per_dim[i])+num_cells_per_dim[i])%num_cells_per_dim[i]);
        }
        
        for(int i = 0; i < current_sample.coordinate.Dimension(); i++)
        {
            current_sample.coordinate[i] = sample.coordinate[i] + (corrected_index[i] - current_index[i])*Size();
        }

        Cell * current_cell = const_cast<Cell *>(_tree.FindCell(Level(), corrected_index));
        
        if(current_cell)
        {
#if 0
            // 09/17/2008: Hmm, I don't think I understand why I need this....
            if(current_cell->NumSamples() > 0)
            {
                // even though mathematically it could contain another sample
                // we still need to remove it from the pool for computation 
                for(unsigned int i = 0; i < verdict_per_class.size(); i++)
                {
                    if(! current_cell->PoolRemove(i))
                    {
                        return 0;
                    }
                }
            }
#endif

            if(! current_cell->DeadZone(current_sample, verdict_per_class))
            {
                return 0;
            }
            else
            {
                for(unsigned int i = 0; i < verdict_per_class.size(); i++)
                {
                    if(verdict_per_class[i])
                    {
                        const int counter_update_status = (add_or_remove > 0 ? current_cell->_conflict_counter.Increment(i) : current_cell->_conflict_counter.Decrement(i));

                        if(! counter_update_status) return 0;

                        if(current_cell->_conflict_counter.Value(i) == 0)
                        {
                            if(! current_cell->PoolAdd(i))
                            {
                                return 0;
                            }
                        }
                        else if(current_cell->_conflict_counter.Value(i) == 1)
                        {
                            if(! current_cell->PoolRemove(i))
                            {
                                return 0;
                            }
                        }
                    }
                }
            }
        }    
    }
    while(counter.Next());

    return 1;
}

int Tree::Cell::AddVisitCount(void)
{
    _visit_count++;
    
    for(unsigned int i = 0; i < _pool_links.size(); i++)
    {
        if(! _tree.AddVisitCount(i, _pool_links[i])) return 0;
    }

    return 1;
}

int Tree::Cell::GetVisitCount(void) const
{
    return _visit_count;
}

int Tree::Cell::GetSamples(vector<const Sample *> & samples) const
{
    samples = _samples;
    return 1;
}
        
int Tree::Cell::GetSubTreeSamples(vector<const Sample *> & samples) const
{
    samples = _samples;
    vector<const Sample *> child_samples;

    for(unsigned int i = 0; i < _children.size(); i++)
    {
        if(! _children[i]->GetSubTreeSamples(child_samples))
        {
            return 0;
        }
        else
        {
            samples.insert(samples.end(), child_samples.begin(), child_samples.end());
        }
    }

    return 1;
}

int Tree::Cell::GetConflicts(const Sample & sample, vector<const Sample *> & neighbors) const
{
    return GetConflicts(sample, neighbors, 1);
}

int Tree::Cell::GetConflicts(const Sample & sample, const int same_tree, vector<const Sample *> & neighbors) const
{
    return GetConflicts(sample, same_tree, neighbors, 1);
}

int Tree::Cell::GetConflicts(const Sample & sample, vector<const Sample *> & neighbors, const int find_all) const
{
    return GetConflicts(sample, 1, neighbors, find_all);
}

int Tree::Cell::GetConflicts(const Sample & sample, const int same_tree, vector<const Sample *> & neighbors, const int find_all) const
{
    const ConflictChecker & checker = _tree.GetConflictChecker(); 
    const float worst_case_distance = checker.MaxDistance();

    neighbors.clear();

    if(worst_case_distance <= 0)
    {
        return 0;
    }
    
    // just check
    if(! Inside(sample))
    {
        throw Exception("sample is NOT inside cell");
        return 0;
    }

    const int dimension = _tree.Dimension();

    const Cell * container = this;
    
    while(container)
    {
        const int cell_offset = CellOffset();
    
        const vector<int> container_index = _index;
        vector<int> current_index(dimension);
        vector<int> corrected_index(dimension);

        vector<int> offset_index(dimension);

        SequentialCounter offset_index_counter(dimension, -cell_offset, cell_offset);

        offset_index_counter.Reset();
                
        vector<int> num_cells_per_dim;
        if(! _tree.GetNumCellsPerDimension(container->Level(), num_cells_per_dim))
        {
            throw Exception("cannot get num cells per dimension");
            return 0;
        }

        do
        {
            offset_index_counter.Get(offset_index);
            assert(offset_index.size() == dimension);

            // test current offset_index
            for(unsigned int i = 0; i < current_index.size(); i++)
            {
                current_index[i] = container_index[i] + offset_index[i];
                corrected_index[i] = (((current_index[i]%num_cells_per_dim[i])+num_cells_per_dim[i])%num_cells_per_dim[i]);
            }
        
            const Cell * current_cell = _tree.FindCell(container->Level(), corrected_index);
        
            if(current_cell)
            {
                vector<const Sample *> subtree_samples;
                if(!same_tree && (container == this))
                {
                    if(! current_cell->GetSubTreeSamples(subtree_samples))
                    {
                        return 0;
                    }
                }

                const vector<const Sample *> & current_cell_samples = ((!same_tree && (container == this)) ? subtree_samples : current_cell->_samples);

                for(unsigned int j = 0; j < current_cell_samples.size(); j++)
                {
                    Sample current_sample = *current_cell_samples[j];

                    for(int i = 0; i < dimension; i++)
                    {
                        // toroidal boundary handling
                        current_sample.coordinate[i] += ((current_index[i]-corrected_index[i])*container->Size());
                    }

                    const ConflictChecker::Status status = checker.Check(sample, current_sample);

                    if(status == ConflictChecker::CHECK_ERROR) throw Exception("error for conflict check");

                    // regarding the second condition:
                    // Q: hmm, why don't I have to do this for Grid?
                    // A: for grid, the cells are small enough to guarantee no problem for this
                    // add samples already in this cell
                    // even though mathematically they might not be in conflict with the query
                    // computational-wise we have to add them
                    // since we allow only one sample per cell
                    if((status == ConflictChecker::CHECK_FAIL) || (same_tree && (current_cell == this) && (status == ConflictChecker::CHECK_PASS)))
                    {
                        neighbors.push_back(current_cell_samples[j]);
                        if(! find_all) return 1;
                    }
                }
            }
        }
        while(offset_index_counter.Next());
    
        // go up one level
        container = container->Parent();
    }

    // done
    return 1;
}

int Tree::Cell::GetChildren(vector<Cell *> & children)
{
    children = _children;
    return 1;
}

int Tree::Cell::GetChildren(vector<const Cell *> & children) const
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

int Tree::Cell::Subdivide(void)
{
    return Subdivide(0); // non-blind
}

int Tree::Cell::Subdivide(const int blind)
{
    if(!blind)
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
                const float sample_min_distance = _tree.GetDistanceField().Query(*_samples[i])/2;
#else
                const float sample_min_distance = _tree.GetDistanceField().Query(*_samples[i]);
#endif
            
                if(cell_size*sqrt(_tree.Dimension()*1.0) <= sample_min_distance)
                {
                    return 0;
                }
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
        // done via Inherit()
        
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

int Tree::Cell::Inherit(void)
{
    if(_children.size() > 0)
    {
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
        // no need to inherit
        return 1;
    }
}

int Tree::Cell::PoolAdd(const int class_id)
{
    return _tree.PoolAdd(class_id, _pool_links[class_id]);
}

int Tree::Cell::PoolRemove(const int class_id)
{
    return _tree.PoolRemove(class_id, _pool_links[class_id]);
}

Tree::Cell::MultiCounter::MultiCounter(void)
{
    // nothing to do
}

Tree::Cell::MultiCounter::~MultiCounter(void)
{
    // nothing to do
}

int Tree::Cell::MultiCounter::Increment(const int class_id)
{
    if(_counts.size() < (class_id+1))
    {
        _counts.resize(class_id+1, 0);
    }

    if((class_id < 0) || (class_id >= _counts.size()))
    {
        return 0;
    }
    else
    {
        _counts[class_id] += 1;
        return 1;
    }
}

int Tree::Cell::MultiCounter::Decrement(const int class_id)
{
    if((class_id < 0) || (class_id >= _counts.size()))
    {
        return 0;
    }
    else
    {
        if(_counts[class_id] > 0)
        {
            _counts[class_id] -= 1;
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

int Tree::Cell::MultiCounter::Value(const int class_id) const
{
    if((class_id < 0) || (class_id >= _counts.size()))
    {
        return 0;
    }
    else
    {
        return _counts[class_id];
    }
}

Tree::Pool::Pool(void) : _visit_count(0)
{
    // nothing to do
}

Tree::Pool::~Pool(void)
{
    // nothing to do
}

int Tree::Pool::Add(CellLink & link)
{
    if(link.position >= 0)
    {
        if(link.position < _links.size())
        {
            return _links[link.position] == &link;
        }
        else
        {
            // inconsistent
            return 0;
        }
    }
    else
    {
        _visit_count += link.cell->GetVisitCount();

        _links.push_back(&link);
        link.position = _links.size()-1;
        return (_links[link.position] == &link);
    }
}

int Tree::Pool::Remove(CellLink & link)
{
    if((link.position >= 0) && (link.position < _links.size()))
    {
        if(_links[link.position] == &link)
        {
            _visit_count -= link.cell->GetVisitCount();
            
            _links[link.position] = _links[_links.size()-1];
            _links[link.position]->position = link.position;
            _links.pop_back();
            link.position = -1;

            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return (link.position < 0);
    }
}

void Tree::Pool::Clear(void)
{
    for(unsigned int i = 0; i < _links.size(); i++)
    {
        _links[i]->position = -1;
    }

    _links.clear();

    _visit_count = 0;
}

int Tree::Pool::GetCells(vector<const Cell *> & cells) const
{
    cells.clear();

    for(unsigned int i = 0; i < _links.size(); i++)
    {
        cells.push_back(_links[i]->cell);
    }

    return 1;
}

int Tree::Pool::NumCells(void) const
{
    return _links.size();
}

const Tree::Cell * Tree::Pool::GetCell(const int which_cell) const
{
    if((which_cell >= 0) && (which_cell < _links.size()))
    {
        return _links[which_cell]->cell;
    }
    else
    {
        return 0;
    }
}

int Tree::Pool::GetVisitCount(void) const
{
    return _visit_count;
}

int Tree::Pool::AddVisitCount(const CellLink & link)
{ 
    if(link.position >= 0)
    {
        if(link.position < _links.size())
        {
            if(_links[link.position] == &link)
            {
                _visit_count++;
                return 1;
            }
            else
            {
                // inconsistent
                return 0;
            }
        }
        else
        {
            // inconsistent
            return 0;
        }
    }
    else
    {
        // link/cell not in the pool
        return 0;
    }
}

Tree::Pool::CellLink::CellLink(const Cell * cell_i) : cell(cell_i), position(-1)
{
    // nothing to do
}

Tree::Pool::CellLink::~CellLink(void)
{
    // nothing to do
}
