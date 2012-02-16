/*
  FairClassWeight.hpp

  weigh class samples so that each splat collection has same impact

  Li-Yi Wei
  05/05/2009

*/

#ifndef _FAIR_CLASS_WEIGHT_HPP
#define _FAIR_CLASS_WEIGHT_HPP

#include "SampleCountClassWeight.hpp"

class FairClassWeight : public SampleCountClassWeight
{
public:
    FairClassWeight(const vector<int> & num_samples_per_class);
    ~FairClassWeight(void);

    float Get(const int class_id0, const int class_id1) const;

    float Get(const vector<int> & source_classes) const;
};
#endif
