/*
  NBallSliceCounter.cpp

  Li-Yi Wei
  07/23/2007

*/

#include <iostream>
using namespace std;

#include <math.h>

#include "NBallSliceCounter.hpp"

NBallSliceCounter::NBallSliceCounter(const int dimension, const float radius_square) : Counter(dimension), _radius_square(radius_square), _radius(sqrt(radius_square)), _limit(dimension), _value_square_sum(dimension), _value(dimension)
{
    Reset();
}

NBallSliceCounter::~NBallSliceCounter(void)
{
    // nothing to do
}

int NBallSliceCounter::Reset(void)
{
    if(_value.size() > 0)
    {
        _value[_value.size()-1] = -static_cast<int>(floor(_radius));
        _value_square_sum[_value_square_sum.size()-1] = 0;
        
        for(int i = _value.size()-2; i >= 0; i--)
        {
            _value_square_sum[i] = _value_square_sum[i+1] + _value[i+1]*_value[i+1];
            _value[i] = -static_cast<int>(floor(sqrt(_radius_square-_value_square_sum[i])));
        }
    }

    for(unsigned int i = 0; i < _limit.size(); i++)
    {
        _limit[i] = -_value[i];
    }

    return 1;
}

int NBallSliceCounter::Get(vector<int> & value) const
{
    value = _value;
    return 1;
}

int NBallSliceCounter::Get(vector<int> & value, float & norm_square) const
{
    value = _value;

    if(_value_square_sum.size() > 0)
    {
        norm_square = _value_square_sum[0] + _value[0]*_value[0];
    }
    else
    {
        norm_square = 0;
    }
    
    return 1;
}
    
int NBallSliceCounter::Next(void)
{
    int which_dim = 0;
    for(which_dim = 0; which_dim < _value.size(); which_dim++)
    {
        if(_value[which_dim] < _limit[which_dim])
        {
            _value[which_dim]++;

            for(int i = which_dim-1; i >= 0; i--)
            {
                _value_square_sum[i] = _value_square_sum[i+1] + _value[i+1]*_value[i+1];
                _value[i] = -static_cast<int>(floor(sqrt(_radius_square - _value_square_sum[i])));
                _limit[i] = - _value[i];
            }
                    
            break;
        }
    }
    
    // done
    return (which_dim < _value.size());
}
    
