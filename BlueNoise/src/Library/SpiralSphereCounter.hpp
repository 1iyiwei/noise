/*
  SpiralSphereCounter.hpp

  Li-Yi Wei
  07/08/2007

*/

#ifndef _SPIRAL_SPHERE_COUNTER_HPP
#define _SPIRAL_SPHERE_COUNTER_HPP

#include "Counter.hpp"

class SpiralSphereCounter : public Counter
{
public:
    SpiralSphereCounter(const int dimension, const int digit_min, const int digit_max);
    ~SpiralSphereCounter(void);

    int Reset(void);

    int Get(vector<int> & value) const;
    
    int Next(void);

    int Stopped(void);
    
protected:
};

#endif
