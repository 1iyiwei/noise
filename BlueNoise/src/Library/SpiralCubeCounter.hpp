/*
  SpiralCubeCounter.hpp

  Li-Yi Wei
  07/08/2007

*/

#ifndef _SPIRAL_CUBE_COUNTER_HPP
#define _SPIRAL_CUBE_COUNTER_HPP

#include "Counter.hpp"

class SpiralCubeCounter : public Counter
{
public:
    SpiralCubeCounter(const int dimension, const int digit_min, const int digit_max);
    ~SpiralCubeCounter(void);

    int Reset(void);

    int Get(vector<int> & value) const;
    
    int Next(void);

    int Stopped(void);

protected:
    static int MaxRadius(const int digit_min, const int digit_max);
    
protected:
    const int _max_radius;
    int _radius;
    vector<int> _value;
};

#endif
