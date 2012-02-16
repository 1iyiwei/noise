/*
  SampleCountClassWeight.hpp

  the base class for all that determine weights solely by class sample count

  Li-Yi Wei
  07/02/2009

*/

#ifndef _SAMPLE_COUNT_CLASS_WEIGHT_HPP
#define _SAMPLE_COUNT_CLASS_WEIGHT_HPP

#include "ClassWeight.hpp"

class SampleCountClassWeight : public ClassWeight
{
public:
    SampleCountClassWeight(const vector<int> & num_samples_per_class);
    virtual ~SampleCountClassWeight(void) = 0;

    virtual float Get(const vector<int> & source_classes) const;

    int NumClass(void) const;

protected:
    const vector<int> _num_samples;
};
#endif
