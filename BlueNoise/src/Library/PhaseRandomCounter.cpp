/*
  PhaseRandomCounter.cpp

  Li-Yi Wei
  07/10/2007

*/

#include <math.h>
#include <assert.h>

#include "PhaseRandomCounter.hpp"
#include "Array.hpp"
#include "SequentialCounter.hpp"
#include "Random.hpp"

PhaseRandomCounter::PhaseRandomCounter(const int dimension, const int digit_min, const int digit_max, const int clique_size) : Counter(dimension), _clique_size(clique_size), _digit_min(dimension), _digit_max(dimension)
{
    for(int i = 0; i < dimension; i++)
    {
        _digit_min[i] = digit_min;
        _digit_max[i] = digit_max;
    }
}

PhaseRandomCounter::~PhaseRandomCounter(void)
{
    // nothing to do
}

int PhaseRandomCounter::Reset(void)
{
    // active list;
    deque< vector<int> > active_list;

    {
        SequentialCounter counter(Dimension(), _digit_min, _digit_max);
        counter.Reset();

        vector<int> foo(Dimension());
        do
        {
            if(! counter.Get(foo)) return 0;

            active_list.push_back(foo);
        }
        while(counter.Next());
        
        deque< vector<int> > random_list;

        while(active_list.size() > 0)
        {
            const int selection = static_cast<int>(floor(Random::UniformRandom()*(active_list.size()-1)));

            random_list.push_back(active_list[selection]);
            active_list[selection] = active_list[0];
            active_list.pop_front();
        }

        active_list = random_list;
    }

    // prepare the array for acceleration queries
    vector<int> array_size(Dimension());
    {
        for(int i = 0; i < Dimension(); i++)
        {
            array_size[i] = _digit_max[i]-_digit_min[i]+1;
        }
    }
    Array<int> id_grid(array_size);
    id_grid.Clear(0);

    // neighbor counter
    SequentialCounter neighbor_counter(Dimension(), -(_clique_size-1), _clique_size-1);
    vector<int> neighbor_offset(Dimension());
    
    // do the work
    _values.clear();

    int current_id = -1;
    
    while(active_list.size() > 0)
    {
        current_id++;
        
        // mark grid cell id
        for(unsigned int i = 0; i < active_list.size(); i++)
        {
            const vector<int> & current_index = active_list[i];

            int current_index_id;
            if(! id_grid.Get(current_index, current_index_id))
            {
                return 0;
            }

            // assert(current_index_id <= current_id);
            
            if(current_index_id == current_id)
            {
#if 0
                // update self id
                if(! id_grid.Put(current_index, current_id))
                {
                    return 0;
                }
#endif
                // update neighbors id
                neighbor_counter.Reset();

		do
                {
                    if(! neighbor_counter.Get(neighbor_offset))
                    {
                        return 0;
                    }

                    for(unsigned int j = 0; j < neighbor_offset.size(); j++)
                    {
                        neighbor_offset[j] = (((neighbor_offset[j]+current_index[j])%array_size[j])+array_size[j])%array_size[j];
                    }

                    int self = 1;
                    {
                        for(unsigned int j = 0; j < neighbor_offset.size(); j++)
                        {
                            if(neighbor_offset[j] != current_index[j]) self = 0;
                        }
                    }
                    if(self) continue;
                    
                    int neighbor_id;
                    if(! id_grid.Get(neighbor_offset, neighbor_id))
                    {
                        return 0;
                    }
                    if(neighbor_id == current_id)
                    {
                        if(! id_grid.Put(neighbor_offset, current_id+1))
                        {
                            return 0;
                        }
                    }
                }
		while(neighbor_counter.Next());
            }
        }
    
        // update lists
        {
            deque< vector<int> > new_active_list;
            
            for(unsigned int i = 0; i < active_list.size(); i++)
            {
                int current_index_id;
                if(! id_grid.Get(active_list[i], current_index_id))
                {
                    return 0;
                }
                
                if(current_index_id == current_id)
                {
                    Entry entry;
                    entry.value = active_list[i];
                    entry.group_id = current_id;

                    _values.push_back(entry);
                }
                else
                {
                    new_active_list.push_back(active_list[i]);
                }
            }

            assert(new_active_list.size() < active_list.size());
            
            active_list = new_active_list;
        }
    }

    // done
    return 1;
}

int PhaseRandomCounter::Get(vector<int> & value) const
{
    if(_values.size() > 0)
    {
        value = _values[0].value;
        return 1;
    }
    else
    {
        return 0;
    }
}

int PhaseRandomCounter::GetID(int & value) const
{
    if(_values.size() > 0)
    {
        value = _values[0].group_id;
        return 1;
    }
    else
    {
        return 0;
    }
}

int PhaseRandomCounter::Next(void)
{
    if(_values.size() > 1)
    {
        _values.pop_front();
        return 1;
    }
    else
    {
        return 0;
    }
}

int PhaseRandomCounter::NumGroups(void) const
{
    if(_values.size() > 0)
    {
        return (_values[_values.size()-1].group_id+1);
    }
    else
    {
        return 0;
    }
}

int PhaseRandomCounter::Check(void) const
{ 
    // prepare the array for acceleration queries
    vector<int> array_size(Dimension());
    {
        for(int i = 0; i < Dimension(); i++)
        {
            array_size[i] = _digit_max[i]-_digit_min[i]+1;
        }
    }
    Array<int> id_grid(array_size);
    id_grid.Clear(-1);

    vector<int> self_index(Dimension());
    int self_id;

    // fill in id_grid
    {
        for(unsigned int i = 0; i < _values.size(); i++)
        {
            self_index = _values[i].value;
            self_id = _values[i].group_id;

            if(! id_grid.Put(self_index, self_id))
            {
                return 0;
            }
        }
    }
    
    // check
    vector<int> neighbor_index(Dimension());
    int neighbor_id;
    SequentialCounter neighbor_counter(Dimension(), -(_clique_size-1), _clique_size-1);
    {    
        for(unsigned int i = 0; i < _values.size(); i++)
        {
            self_index = _values[i].value;
            self_id = _values[i].group_id;

            neighbor_counter.Reset();

            do
            {
                if(! neighbor_counter.Get(neighbor_index))
                {
                    return 0;
                }

                {
                    for(unsigned int j = 0; j < neighbor_index.size(); j++)
                    {
                        neighbor_index[j] = ((neighbor_index[j]+self_index[j])%array_size[j]+array_size[j])%array_size[j];
                    }
                }

                int self = 1;
                {
                    for(unsigned int j = 0; j < neighbor_index.size(); j++)
                    {
                        if(neighbor_index[j] != self_index[j]) self = 0;
                    }
                }

                if(self) continue;

                if(! id_grid.Get(neighbor_index, neighbor_id))
                {
                    return 0;
                }

                if(self_id == neighbor_id)
                {
                    return 0;
                }
            }
            while(neighbor_counter.Next());
        }
    }
    
    // done
    return 1;
}
