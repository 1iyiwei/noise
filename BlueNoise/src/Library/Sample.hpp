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
    Sample(const int dim);
    ~Sample(void);
        
public:
    float value;
    Coordinate coordinate;
};

#endif
