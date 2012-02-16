/*
  GridEnergySampleStore.cpp

  Li-Yi Wei
  07/08/2009

*/

#include <math.h>

#include "GridEnergySampleStore.hpp"
#include "Exception.hpp"

GridEnergySampleStore::GridEnergySampleStore(const Domain & domain, const GridSpec & grid_spec, const UniformKernelSize & kernel_size, const float kernel_radius_scale) : /*_domain(domain),*/ _grid(domain, grid_spec), _radius_store(kernel_size, kernel_radius_scale)
{
    // nothing else to do
}

GridEnergySampleStore::~GridEnergySampleStore(void)
{
    // nothing else to do
}
    
GridEnergySampleStore * GridEnergySampleStore::Clone(void) const
{
    // probably only allows clones when the grid is totally empty
    // but be lazy for now
    throw Exception("GridEnergySampleStore::Clone(): shouldn't be called");
    return 0;
}

int GridEnergySampleStore::Set(const vector<EnergySample> & samples)
{
    if(! EnergySampleStore::Set(samples)) return 0;

    // clear grid
    _grid.Clear();

    // do not access samples directly below; instead, access parent class
    for(unsigned int i = 0; i < _p_samples.size(); i++)
    {
        if(! _grid.Plus(_p_samples[i]))
        {
            throw Exception("GridEnergySampleStore::Set(): cannot add sample pointer");
            return 0;
        }
    }
    
    // done
    return 1;
}

const vector<const EnergySample *> & GridEnergySampleStore::Get(const Sample & query) const
{
    return _grid.Get(query, _radius_store.Get(query));
}

const vector<EnergySample *> & GridEnergySampleStore::Get(const Sample & query)
{
    return _grid.Get(query, _radius_store.Get(query));
}

int GridEnergySampleStore::Plus(const EnergySample & rookie)
{
    if(! EnergySampleStore::Plus(rookie)) return 0;

    if(! rookie.GetSample(_rookie_sample)) return 0;

    // if(! _domain.Inside(_rookie_sample)) return 0;

    int rookie_index = -1;
    // assuming rookie is added to the back of the queue
    for(int i = static_cast<int>(_samples.size())-1; i >= 0; i--)
    {
        if(! _samples[i].GetSample(_sample)) return 0;

        if(_sample == _rookie_sample)
        {
            rookie_index = i;
            break;
        }
    }

    if((rookie_index >= 0) && (rookie_index < _p_samples.size()))
    {
        return _grid.Plus(_p_samples[rookie_index]);
    }
    else
    {
        throw Exception("GridEnergySampleStore::Plus(): illegal rookie index");
        return 0;
    }
}

int GridEnergySampleStore::Minus(const int retiree_index)
{
    if((retiree_index < 0) || (retiree_index >= _p_samples.size()))
    {
        throw Exception("GridEnergySampleStore::Minus(): illegal retiree index");
        return 0;
    }
    else
    {
        if(! _grid.Minus(_p_samples[retiree_index])) return 0;

        // victim_index is consistency with EnergySampleStore::Minus()
        const int victim_index = _samples.size()-1; 
        _victim = _samples[victim_index];
        if(retiree_index != victim_index)
        {
            if(! _grid.Minus(_p_samples[victim_index])) return 0;
        }

        if(! EnergySampleStore::Minus(retiree_index)) return 0;

        if(retiree_index != victim_index)
        {
            // check
            if(! _victim.GetSample(_victim_sample)) return 0;
            if(! _samples[retiree_index].GetSample(_sample)) return 0;
            if(_victim_sample != _sample)
            {
                throw Exception("GridEnergySampleStore::Minus(): inconsistent victim");
                return 0;
            }

            // add back victim
            if(! _grid.Plus(_p_samples[retiree_index])) return 0;
        }

        // done
        return 1;
    }
}

int GridEnergySampleStore::Move(EnergySample & source, const Sample & target)
{
    if(! _grid.Minus(&source))
    {
        throw Exception("GridEnergySampleStore::Move(): cannot minus source");
        return 0;
    }

    if(! EnergySampleStore::Move(source, target))
    {
        return 0;
    }

    if(! _grid.Plus(&source))
    {
        throw Exception("GridEnergySampleStore::Move(): cannot plus source");
        return 0;
    }

    // done
    return 1;
}

GridEnergySampleStore::RadiusStore::RadiusStore(const UniformKernelSize & kernel_size, const float kernel_radius_scale) : _class_radius(kernel_size.NumClass()), _radius(0)
{
    Sample sample;

    for(unsigned int i = 0; i < _class_radius.size(); i++)
    {
        sample.id = i;

#ifdef _USE_2_CLASS_ENERGY
        _class_radius[i] = kernel_size.Get(sample, i)*kernel_radius_scale;
#else
        vector<int> class_option(_class_radius.size(), 0);
        class_option[i] = 1;
        
        _class_radius[i] = kernel_size.Get(sample, class_option)*kernel_radius_scale;
#endif
        if(_radius < _class_radius[i])
        {
            _radius = _class_radius[i];
        }
    }
}
        
float GridEnergySampleStore::RadiusStore::Get(const Sample & query) const
{
    if(query.id < 0)
    {
        return _radius;
    }
    else if(query.id < _class_radius.size())
    {
        return _class_radius[query.id];
    }
    else
    {
        // error
        throw Exception("GridEnergySampleStore::RadiusStore::Get(): illegal situation");
        return 0;
    }
}

GridEnergySampleStore::Grid::Grid(const Domain & domain, const GridSpec & spec) : _domain(domain), _array(spec.domain_size), _cell_size(spec.cell_size), _counter(domain.Dimension(), 0, 0)
{
    // spec must contain the bounding box of domain
    vector<float> bounding_box;
    if(!domain.BoundingBox(bounding_box))
    {
        throw Exception("GridEnergySampleStore::GridEnergySampleStore(): cannot get domain bounding box");
    }

    if(bounding_box.size() != spec.domain_size.size())
    {
        throw Exception("GridEnergySampleStore::GridEnergySampleStore(): incompatible domain and spec dimension");
    }

    for(unsigned int i = 0; i < bounding_box.size(); i++)
    {
        if(bounding_box[i] > spec.domain_size[i]*spec.cell_size)
        {
            // throw Exception("GridEnergySampleStore::GridEnergySampleStore(): domain not inside spec");
        }
    }
}

GridEnergySampleStore::Grid::~Grid(void)
{
    // nothing else to do
}

void GridEnergySampleStore::Grid::Clear(void)
{
    GridElement empty;
    _array.Clear(empty);
}

const vector<const EnergySample *> & GridEnergySampleStore::Grid::Get(const Sample & query, const float radius) const
{
    return GetNeighbors(query, radius);
}

const vector<EnergySample *> & GridEnergySampleStore::Grid::Get(const Sample & query, const float radius)
{ 
    const vector<const EnergySample *> & sample_list = GetNeighbors(query, radius);

    _sample_list = vector<EnergySample *>(sample_list.size());

    for(unsigned int i = 0; i < _sample_list.size(); i++)
    {
        _sample_list[i] = const_cast<EnergySample *>(sample_list[i]);
    }

    return _sample_list;
}

const vector<const EnergySample *> & GridEnergySampleStore::Grid::GetNeighbors(const Sample & query, const float radius) const
{
    if(_array.Dimension() != query.coordinate.Dimension())
    {
        throw Exception("GridEnergySampleStore::Grid::GetNeighbors(): dimension mismatch");
    }

    const float radius2 = radius*radius;
    const int grid_radius = static_cast<int>(ceil(radius/_cell_size))+2;

    if(!_counter.Reset(_array.Dimension(), -grid_radius, grid_radius))
    {
        throw Exception("GridEnergySampleStore::Grid::GetNeighbors(): cannot reset counter");
    }

    _const_sample_list.clear();

    if(! FindCell(query, _center_index))
    {
        throw Exception("GridEnergySampleStore::Grid::GetNeighbors(): cannot find center index");
    }
    
    do
    {
        _counter.Get(_original_index);

        _corrected_index = _original_index;
        for(unsigned int i = 0; i < _corrected_index.size(); i++)
        {
            _corrected_index[i] = ((_center_index[i] + _original_index[i])%_array.Size(i)+_array.Size(i))%_array.Size(i);
        }

        if(! _array.Get(_corrected_index, _element_list))
        {
            throw Exception("GridEnergySampleStore::Grid::GetNeighbors(): cannot get array element");
        }

        for(unsigned int i = 0; i < _element_list.size(); i++)
        {
            if(! _element_list[i]->GetSample(_original_neighbor))
            {
                throw Exception("GridEnergySampleStore::Grid::GetNeighbors(): cannot get original neighbor sample");
            }

            if(! _domain.Nearest(query, _original_neighbor, _corrected_neighbor))
            {
                throw Exception("GridEnergySampleStore::Grid::GetNeighbors(): cannot get corrected neighbor sample");
            }

            float distance2 = 0;
            for(int j = 0; j < _corrected_neighbor.coordinate.Dimension(); j++)
            {
                const float diff = _corrected_neighbor.coordinate[j]-query.coordinate[j];
                distance2 += diff*diff;
            }

            if(distance2 <= radius2)
            {
                int already_there = 0;

                for(unsigned int j = 0; j < _const_sample_list.size(); j++)
                {
                    if(_const_sample_list[j] == _element_list[i])
                    {
                        already_there = 1; break;
                    }
                }
                
                // add
                if(! already_there)
                {
                    _const_sample_list.push_back(_element_list[i]);
                }
            }
        }
    }
    while(_counter.Next());

    return _const_sample_list;
}

int GridEnergySampleStore::Grid::Plus(const EnergySample * sample)
{
    return PlusMinus(sample, 0);
}

int GridEnergySampleStore::Grid::Minus(const EnergySample * sample)
{
    return PlusMinus(sample, 1);
}

int GridEnergySampleStore::Grid::PlusMinus(const EnergySample * sample, const int plus_or_minus)
{
    if(! FindCell(*sample, _cell_index))
    {
        throw Exception("GridEnergySampleStore::Grid::PlusMinus(): cannot find cell");
        return 0;
    }

    GridElement element;

    if(! _array.Get(_cell_index, element))
    {
        throw Exception("GridEnergySampleStore::Grid::PlusMinus(): cannot get array element");
        return 0;
    }

    int index = -1;
    for(unsigned int i = 0; i < element.size(); i++)
    {
        if(sample == element[i]) index = i;
    }

    if(!plus_or_minus)
    {
        if(index < 0)
        {
            element.push_back(sample);
        }
        else
        {
            // already there
            throw Exception("GridEnergySampleStore::Grid::PlusMinus(): cannot add already existing");
            return 0;
        }
    }
    else if(plus_or_minus)
    {
        if(index >= 0)
        {
            element[index] = element[element.size()-1];
            element.pop_back();
        }
        else
        {
            // not there
            throw Exception("GridEnergySampleStore::Grid::PlusMinus(): cannot remove not there");
            return 0;
        }
    }

    return _array.Put(_cell_index, element);
}

int GridEnergySampleStore::Grid::FindCell(const EnergySample & query, vector<int> & cell_index) const
{
    if(! query.GetSample(_sample))
    {
        throw Exception("GridEnergySampleStore::Grid::FindCell(): cannot obtain query sample");
        return 0;
    }

    return FindCell(_sample, cell_index);
}

int GridEnergySampleStore::Grid::FindCell(const Sample & query, vector<int> & cell_index) const
{
    if(query.coordinate.Dimension() != _array.Dimension())
    {
        throw Exception("GridEnergySampleStore::Grid::FindCell(): illegal query coordinate dimension");
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
            throw Exception("GridEnergySampleStore::Grid::FindCell(): cell_index out of bound");
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
