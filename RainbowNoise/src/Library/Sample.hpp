/*
  Sample.hpp

  a sample with value and coordinate
  
  Li-Yi Wei
  06/28/2007

*/

#ifndef _SAMPLE_HPP
#define _SAMPLE_HPP

#include "Coordinate.hpp"

typedef CoordinateT<float> Coordinate;

class Sample
{
public:
    Sample(void);
    Sample(const int dim);
    ~Sample(void);
        
public:
    int id; // class id, actually
    float value;
    float r;
    Coordinate coordinate;
};

int operator==(const Sample & s1, const Sample & s2);
int operator!=(const Sample & s1, const Sample & s2);

// squared distance between 2 samples
float Distance2(const Sample & s1, const Sample & s2);

#endif
