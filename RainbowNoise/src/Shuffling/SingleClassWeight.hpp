/*
  SingleClassWeight.hpp

  weigh class samples so that only single classes are considered

  Li-Yi Wei
  07/02/2009

*/

#ifndef _SINGLE_CLASS_WEIGHT_HPP
#define _SINGLE_CLASS_WEIGHT_HPP

#include "SampleCountClassWeight.hpp"

class SingleClassWeight : public SampleCountClassWeight
{
public:
    SingleClassWeight(const vector<int> & num_samples_per_class);
    ~SingleClassWeight(void);

    float Get(const int class_id0, const int class_id1) const;

    float Get(const vector<int> & source_classes) const;
};
#endif
