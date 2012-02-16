/*
  HybridDiffGenerator.hpp

  hybrid diff generator for needles over white noise

  Li-Yi Wei
  11/04/2008

*/

#ifndef _HYBRID_DIFF_GENERATOR_HPP
#define _HYBRID_DIFF_GENERATOR_HPP

#include "DiffGenerator.hpp"

class HybridDiffGenerator : public DiffGenerator
{
public:
    // r_value: positive for removing diff < r_value, negative for keep them
    // r_variance: positive for turning diff < r_value into >, negative for not
    HybridDiffGenerator(const vector<float> & domain_size, const float r_value, const float r_variance);
    ~HybridDiffGenerator(void);

    int Produce(vector<Sample> & differences) const;

protected:
    // return the number of valid entries
    int UnitProduce(vector<Sample> & results) const;

protected:
    const float _r_value;
    const float _r_variance;
    const bool _keep_diff_less_r;
    const bool _spawn_diff_less_r;

    mutable Coordinate _coord0, _coord1;
};

#endif

 
