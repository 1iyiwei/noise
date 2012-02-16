/*
  OnionLayerCounter.cpp

  Li-Yi Wei
  07/17/2007

*/

#include <vector>
#include <deque>
#include <algorithm>
using namespace std;

#include <math.h>
#include <assert.h>

#include "OnionLayerCounter.hpp"
#include "SequentialCounter.hpp"
#include "NBallSliceCounter.hpp"

int OnionLayerCounter::Sortee::operator<(const Sortee & another) const
{
    return (key < another.key);
}

int OnionLayerCounter::TotalCount(const int dimension, const float radius_square)
{
    const int radius_int = static_cast<int>(floor(sqrt(radius_square)));
    
    SequentialCounter counter(dimension, -radius_int, radius_int);

    counter.Reset();

    int count = 0;
    vector<int> index(dimension);
    
    do
    {
        counter.Get(index);
        
        int total = 0;
        for(unsigned int i = 0; i < index.size(); i++)
        {
            total += index[i]*index[i];
        }
              
        if(total <= radius_square)
        {
            count++;
        }
    }
    while(counter.Next());

    return count;
}

OnionLayerCounter::OnionLayerCounter(const int dimension, const float radius_square) : Counter(dimension)
{
    PostalSortConstruction(dimension, radius_square, _values);
    // BruteForceSortConstruction(dimension, radius_square, _values);
}

OnionLayerCounter::~OnionLayerCounter(void)
{
    // nothing to do
}

int OnionLayerCounter::Reset(void)
{
    _pointer = 0;

    return 1;
}

int OnionLayerCounter::Get(vector<int> & value) const
{
    if( (_pointer >= 0) && (_pointer < _values.size()))
    {
        value = _values[_pointer];
        return 1;
    }
    else
    {
        return 0;
    }
}

int OnionLayerCounter::Next(void)
{
    if(_pointer >= (_values.size()-1))
    {
        return 0;
    }
    else
    {
        _pointer++;
        return 1;
    }
}

void OnionLayerCounter::BruteForceSortConstruction(const int dimension, const float radius_square, vector< vector<int> > & values)
{ 
    const int radius_int = static_cast<int>(floor(sqrt(radius_square)));
    
    SequentialCounter counter(dimension, -radius_int, radius_int);

    counter.Reset();

    // accumulate values first
    vector<Sortee> sortees;
    Sortee candidate;
    do
    {
        counter.Get(candidate.value);
        {
            candidate.key = 0;
            for(unsigned int i = 0; i < candidate.value.size(); i++)
            {
                candidate.key += candidate.value[i]*candidate.value[i];
            }
        }
        
        if(candidate.key <= radius_square)
        {
            sortees.push_back(candidate);
        }
    }
    while(counter.Next());

    // sorting
#if 0
    vector<int> vec;
    vec.push_back (10);
    vec.push_back (3);
    vec.push_back (7);

    sort(vec.begin(), vec.end());
#endif
    
    sort(sortees.begin(), sortees.end());
    
    // assign
    values = vector< vector<int> >(sortees.size());

    for(unsigned int i = 0; i < values.size(); i++)
    {
        values[i] = sortees[i].value;
    }
}

void OnionLayerCounter::PostalSortConstruction(const int dimension, const float radius_square, vector< vector<int> > & values)
{
    NBallSliceCounter counter(dimension, radius_square);

    deque< vector< vector<int> > > postal(static_cast<int>(ceil(radius_square))+1);

    counter.Reset();

    vector<int> value(dimension);
    float norm_square;

    do
    {
        counter.Get(value, norm_square);

        const int index = static_cast<int>(norm_square);

        assert((index >= 0) && (index < postal.size()));
        
        postal[index].push_back(value);
    }
    while(counter.Next());

    values.clear();

    while(postal.size() > 0)
    {
        for(unsigned int i = 0; i < postal[0].size(); i++)
        {
            values.push_back(postal[0][i]);
        }

        postal.pop_front();
    }
}
