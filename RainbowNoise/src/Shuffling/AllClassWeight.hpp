/*
  AllClassWeight.hpp

  weigh class samples so that only the entire union of all classes is considered

  Li-Yi Wei
  07/02/2009

*/

#ifndef _ALL_CLASS_WEIGHT_HPP
#define _ALL_CLASS_WEIGHT_HPP

#include "SampleCountClassWeight.hpp"

class AllClassWeight : public SampleCountClassWeight
{
public:
    AllClassWeight(const vector<int> & num_samples_per_class);
    ~AllClassWeight(void);

    float Get(const int class_id0, const int class_id1) const;

    float Get(const vector<int> & source_classes) const;
};
#endif
