/*
  RainbowClassWeight.hpp

  non-zero weigh for only the entire union and individual classes
  match up the mult-class dart throwing merit

  Li-Yi Wei
  07/19/2009

*/

#ifndef _RAINBOW_CLASS_WEIGHT_HPP
#define _RAINBOW_CLASS_WEIGHT_HPP

#include "SampleCountClassWeight.hpp"

class RainbowClassWeight : public SampleCountClassWeight
{
public:
    RainbowClassWeight(const vector<int> & num_samples_per_class);
    ~RainbowClassWeight(void);

    float Get(const int class_id0, const int class_id1) const;

    float Get(const vector<int> & source_classes) const;
};
#endif
