/*
  NeedleDiffGenerator.hpp

  projection from randomly oriented needles
  (simulating very peaky Poisson-disk like distributions)

  Li-Yi Wei
  10/31/2008

*/

#ifndef _NEEDLE_DIFF_GENERATOR_HPP
#define _NEEDLE_DIFF_GENERATOR_HPP

#include "DiffGenerator.hpp"

class NeedleDiffGenerator : public DiffGenerator
{
public:
    NeedleDiffGenerator(const vector<float> & domain_size, const int needle_dimension, const float needle_length);
    NeedleDiffGenerator(const vector<float> & domain_size, const int needle_dimension, const float needle_length, const int needle_length_max_multiple);
    ~NeedleDiffGenerator(void);

    int Produce(vector<Sample> & differences) const;

protected:
    int Produce(Sample & difference) const;

    const int _needle_dimension;
    const float _needle_length;
    const int _needle_length_max_multiple;

    mutable Coordinate _coord;
};

#endif

 
