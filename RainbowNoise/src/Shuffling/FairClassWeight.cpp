/*
  FairClassWeight.cpp

  Li-Yi Wei
  05/05/2009

*/

#include "FairClassWeight.hpp"
// #define _ZERO_WEIGHT_FOR_ALL_ZERO

FairClassWeight::FairClassWeight(const vector<int> & num_samples_per_class) : SampleCountClassWeight(num_samples_per_class)
{
    // nothing to do
}

FairClassWeight::~FairClassWeight(void)
{
    // nothing to do
}

float FairClassWeight::Get(const int class_id0, const int class_id1) const
{
#ifdef _ZERO_WEIGHT_FOR_ALL_ZERO
#error _ZERO_WEIGHT_FOR_ALL_ZERO not suppported in FairClassWeight
#endif

#ifdef _UNITY_WEIGHT
    const float weight = 1.0;
#else
    int all_sum = 0;
    int partial_sum = 0;
    for(unsigned int i = 0; i < _num_samples.size(); i++)
    {
        all_sum += _num_samples[i];

        if((i == class_id0) || (i == class_id1))
        {
            partial_sum += _num_samples[i];
        }
    }
    
    const float weight = (partial_sum > 0 ? all_sum*1.0/partial_sum : 0);
#endif

    return weight;
}

float FairClassWeight::Get(const vector<int> & source_classes) const
{
    if(source_classes.size() != _num_samples.size())
    {
        return -1;
    }

#ifdef _UNITY_WEIGHT
    const float weight = 1.0;
#ifdef _ZERO_WEIGHT_FOR_ALL_ZERO
    int count = 0;
    for(unsigned int i = 0; i < source_classes.size(); i++)
    {
        count += (source_classes[i] > 0);
    }
#endif
#else
#ifdef _ZERO_WEIGHT_FOR_ALL_ZERO
    int count = 0;
#endif
    int all_sum = 0;
    int partial_sum = 0;
    for(unsigned int i = 0; i < source_classes.size(); i++)
    {
#ifdef _ZERO_WEIGHT_FOR_ALL_ZERO
        count += (source_classes[i] > 0);
#endif
        all_sum += _num_samples[i];
        partial_sum += (source_classes[i] > 0)*_num_samples[i];
    }

    const float weight = (partial_sum > 0 ? all_sum*1.0/partial_sum : 0);
#endif

#ifdef _ZERO_WEIGHT_FOR_ALL_ZERO
    return weight*(count > 0);
#else
    return weight;
#endif
}
