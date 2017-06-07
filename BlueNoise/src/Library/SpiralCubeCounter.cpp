/*
  SpiralCubeCounter.cpp

  Li-Yi Wei
  07/08/2007
  
*/

#include "SpiralCubeCounter.hpp"

SpiralCubeCounter::SpiralCubeCounter(const int dimension, const int digit_min, const int digit_max) : Counter(dimension, digit_min, digit_max), _max_radius(MaxRadius(digit_min, digit_max)), _radius(0), _value(dimension)
{
    Reset();
}

SpiralCubeCounter::~SpiralCubeCounter(void)
{
    // nothing to do
}

int SpiralCubeCounter::Reset(void)
{
    _radius = 0;

    for(unsigned int i = 0; i < _value.size(); i++)
    {
        _value[i] = (_digit_min + _digit_max)/2;
    }

    return 1;
}

int SpiralCubeCounter::Get(vector<int> & value) const
{
    value = _value;
    return 1;
}

int SpiralCubeCounter::Next(void)
{
    if(Stopped())
    {
        return 0;
    }
    else
    {
        // always stick with the original direction if possible
        // when need to change direction
        // always pick the one with farther away border
        // without reverting the previous direction

        // this probably won't work beyond 2D....
        
        // not yet done
        return 0;
    }
}

int SpiralCubeCounter::Stopped(void)
{
    return (_radius > _max_radius);
}

int SpiralCubeCounter::MaxRadius(const int digit_min, const int digit_max)
{
    const int middle = (digit_min+digit_max)/2;

    const int right = (digit_max-middle);
    const int left = (middle-digit_min);

    return (right > left ? right : left);
}
