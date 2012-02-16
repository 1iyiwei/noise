/*
  GridEnergySampleTracker.cpp

  Li-Yi Wei
  09/03/2009

*/

#include <algorithm>
using namespace std;

#include <math.h>

#include "GridEnergySampleTracker.hpp"
#include "SequentialCounter.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Exception.hpp"

GridEnergySampleTracker::GridEnergySampleTracker(const GridSpec & domain_spec, const int class_id_min, const int class_id_max) : _class_id_min(class_id_min), _class_id_max(class_id_max), _array(domain_spec.domain_size), _cell_size(domain_spec.cell_size)
{
    if(class_id_min > class_id_max)
    {
        throw Exception("GridEnergySampleTracker::GridEnergySampleTracker(): class_id_min must be <= class_id_max");
    }

    vector<const EnergySample *> empty;
    if(! Set(empty))
    {
        throw Exception("ridEnergySampleTracker::GridEnergySampleTracker(): cannot set empty sample set");
    }
}

GridEnergySampleTracker::~GridEnergySampleTracker(void)
{
    // nothing else to do
}

int GridEnergySampleTracker::Set(const vector<const EnergySample *> & samples)
{
    // set up array
    const GridElement empty = vector< vector<const EnergySample *> >(_class_id_max - _class_id_min + 1);

    _array.Clear(empty);

    GridElement grid_element;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        const EnergySample * p_sample = samples[i];

        if(! p_sample)
        {
            throw Exception("GridEnergySampleTracker::Set(): null sample");
            return 0;
        }

        const EnergySample & sample = *p_sample;

        if(! FindCell(sample, _cell_index))
        {
            throw Exception("GridEnergySampleTracker::Set(): cannot find cell");
            return 0;
        }

        if(! _array.Get(_cell_index, grid_element))
        {
            throw Exception("GridEnergySampleTracker::Set(): cannot get element");
            return 0;
        }

        if(! sample.GetSample(_sample))
        {
            throw Exception("GridEnergySampleTracker::Set(): cannot get sample");
            return 0;
        }

        const int internal_id = MapClassID(_sample.id);

        if((internal_id < 0) || (internal_id > grid_element.size()))
        {
            throw Exception("GridEnergySampleTracker::Set(): illegal internal id");
            return 0;
        }

        for(unsigned int j = 0; j < grid_element[internal_id].size(); j++)
        {
            if(grid_element[internal_id][j] == p_sample)
            {
                throw Exception("GridEnergySampleTracker::Set(): duplication");
                return 0;
            }
        }

        grid_element[internal_id].push_back(p_sample);

        if(! _array.Put(_cell_index, grid_element))
        {
            throw Exception("GridEnergySampleTracker::Set(): cannot put element");
            return 0;
        }
    }

    // set up pools
    _pools = vector< vector<IndexElement> >(_class_id_max - _class_id_min + 1);

    _array.Size(_cell_index);
    for(unsigned int i = 0; i < _cell_index.size(); i++)
    {
        _cell_index[i] -= 1;
    }

    SequentialCounter counter(_cell_index.size(), vector<int>(_cell_index.size(), 0), _cell_index);
    counter.Reset();
    
    IndexElement index_element;

    do
    {
        counter.Get(_cell_index);

        if(! _array.Get(_cell_index, grid_element))
        {
            throw Exception("GridEnergySampleTracker::Set(): cannot get element");
            return 0;
        }

        for(unsigned int i = 0; i < grid_element.size(); i++)
        {
            if(grid_element[i].size() <= 0)
            {
                // add to the pool
                index_element.index = _cell_index;

                _pools[i].push_back(index_element);
            }
        }
    }
    while(counter.Next());

    // sorting
    for(unsigned int i = 0; i < _pools.size(); i++)
    {
        sort(_pools[i].begin(), _pools[i].end());
    }

    // done
    return 1;
}

const EnergySample * GridEnergySampleTracker::Peak(void) const
{
    // rely on the energy field class to find peaks
    return 0;
}

const EnergySample * GridEnergySampleTracker::Trough(const EnergySample & peak) const
{
    // no tracked samples dedicated to troughs
    return 0;
}

int GridEnergySampleTracker::Trough(Sample & sample) const
{
    const int internal_id = MapClassID(sample.id);

    if((internal_id < 0) || (internal_id >= _pools.size()))
    {
        return 0;
    }

    const vector<IndexElement> & pool = _pools[internal_id];

    if(pool.size() <= 0)
    {
        return 0;
    }

    const int selection = static_cast<int>(floor(Random::UniformRandom()*pool.size()))%pool.size();

    const vector<int> & index = pool[selection].index;

    if(sample.coordinate.Dimension() != index.size())
    {
        sample.coordinate = Coordinate(index.size());
    }

    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        sample.coordinate[i] = (Random::UniformRandom() + index[i])*_cell_size;
    }

    // done
    return 1;
}

vector<int> GridEnergySampleTracker::GridSize(const int dimension, const int num_samples_per_class, const int scale)
{
    if(scale < 1)
    {
        throw Exception("GridEnergySampleTracker::GridSize(): scale must be >= 1");
    }

    const float pd_cell_size = Math::ComputeMaxMinDistance(dimension, num_samples_per_class)/sqrt(dimension*1.0);

    // const int size_1d = scale * max(1, static_cast<int>(ceil(pow(num_samples_per_class*1.0, 1.0/dimension))));

    const int size_1d = scale * max(1, static_cast<int>(ceil(1.0/pd_cell_size)));

    return vector<int>(dimension, size_1d);
}

int GridEnergySampleTracker::PlusMinus(const EnergySample & sample, const PlusMinusOption plus_or_minus)
{
    if(! FindCell(sample, _cell_index))
    {
        throw Exception("GridEnergySampleTracker::PlusMinus(): cannot find cell");
        return 0;
    }

    GridElement element;

    if(! _array.Get(_cell_index, element))
    {
        throw Exception("GridEnergySampleTracker::PlusMinus(): cannot get array element");
        return 0;
    }

    if(! sample.GetSample(_sample))
    {
        throw Exception("GridEnergySampleTracker::PlusMinus(): cannot get sample");
        return 0;
    }

    const int internal_id = MapClassID(_sample.id);

    if((internal_id < 0) || (internal_id >= element.size()) || (internal_id >= _pools.size()))
    {
        throw Exception("GridEnergySampleTracker::PlusMinus(): illegal id");
        return 0;
    }

    vector<const EnergySample *> & element_list = element[internal_id];

    int index = -1;
    for(unsigned int i = 0; i < element_list.size(); i++)
    {
        if(&sample == element_list[i]) index = i;
    }

    vector<IndexElement> & pool = _pools[internal_id];

    if(plus_or_minus == PLUS)
    {
        if(index < 0)
        {
            element_list.push_back(&sample);
        }
        else
        {
            // already there
            throw Exception("GridEnergySampleTracker::PlusMinus(): cannot add already existing");
            return 0;
        }

        if(element_list.size() == 1)
        {
            // remove it from the pool
            _index_element.index = _cell_index;
            pair<vector<IndexElement>::iterator, vector<IndexElement>::iterator> index = equal_range(pool.begin(), pool.end(), _index_element);

            if(((index.first+1) != index.second) || (index.first < pool.begin()) || (index.first >= pool.end()))
            {
                throw Exception("GridEnergySampleTracker::PlusMinus(): cannot remove element from pool");
                return 0;
            }

            pool.erase(index.first);
        }
    }
    else if(plus_or_minus == MINUS)
    {
        if(index >= 0)
        {
            element_list[index] = element_list[element_list.size()-1];
            element_list.pop_back();
        }
        else
        {
            // not there
            throw Exception("GridEnergySampleTracker::PlusMinus(): cannot remove not there");
            return 0;
        }

        if(element_list.size() == 0)
        {
            // add it to the pool
            _index_element.index = _cell_index;
            vector<IndexElement>::iterator index = lower_bound(pool.begin(), pool.end(), _index_element);
            pool.insert(index, _index_element);
        }
    }

    return _array.Put(_cell_index, element);
}

int GridEnergySampleTracker::MapClassID(const int external_class_id) const
{
    if((external_class_id < _class_id_min) || (external_class_id > _class_id_max))
    {
        return -1;
    }
    else
    {
        return (external_class_id - _class_id_min);
    }
}

int GridEnergySampleTracker::FindCell(const EnergySample & query, vector<int> & cell_index) const
{
    if(! query.GetSample(_sample))
    {
        throw Exception("GridEnergySampleTracker::FindCell(): cannot obtain query sample");
        return 0;
    }

    return FindCell(_sample, cell_index);
}

int GridEnergySampleTracker::FindCell(const Sample & query, vector<int> & cell_index) const
{
    if(query.coordinate.Dimension() != _array.Dimension())
    {
        throw Exception("GridEnergySampleTracker::FindCell(): illegal query coordinate dimension");
        return 0;
    }

    if(cell_index.size() != _array.Dimension())
    {
        cell_index = vector<int>(_array.Dimension());
    }

    for(unsigned int i = 0; i < cell_index.size(); i++)
    {
        cell_index[i] = static_cast<int>(floor(query.coordinate[i]/_cell_size));

#if 1
        // no outside samples
        if((cell_index[i] < 0) || (cell_index[i] >= _array.Size(i)))
        {
            throw Exception("GridEnergySampleTracker::FindCell(): cell_index out of bound");
            return 0;
        }
#else
        // assuming the sample is inside the grid
        if(cell_index[i] < 0) cell_index[i] = 0;
        if(cell_index[i] >= _array.Size(i)) cell_index[i] = _array.Size(i)-1;
#endif
    }

    return 1;
}

int GridEnergySampleTracker::IndexElement::operator<(const IndexElement & rhs) const
{
    if(index.size() != rhs.index.size())
    {
        throw Exception("GridEnergySampleTracker::IndexElement::operator<(): size mismatch");
        return 0;
    }

    for(unsigned int i = 0; i < index.size(); i++)
    {
        if(index[i] < rhs.index[i])
        {
            return 1;
        }
        else if(index[i] > rhs.index[i])
        {
            return 0;
        }
    }

    return 0;
}
