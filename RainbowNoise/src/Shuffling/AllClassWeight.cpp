/*
  AllClassWeight.cpp

  Li-Yi Wei
  07/02/2009

*/

#include "AllClassWeight.hpp"

AllClassWeight::AllClassWeight(const vector<int> & num_samples_per_class) : SampleCountClassWeight(num_samples_per_class)
{
    // nothing to do
}

AllClassWeight::~AllClassWeight(void)
{
    // nothing to do
}

float AllClassWeight::Get(const int class_id0, const int class_id1) const
{
    const int count = ((0 <= class_id0) && (class_id0 < _num_samples.size())) * ((0 <= class_id1) && (class_id1 < _num_samples.size())) * (class_id0 != class_id1);

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
    
    return weight*(count == 1);
}

float AllClassWeight::Get(const vector<int> & source_classes) const
{
    if(source_classes.size() != _num_samples.size())
    {
        return -1;
    }

#ifdef _UNITY_WEIGHT
    const float weight = 1.0;
    int count = 1;
    for(unsigned int i = 0; i < source_classes.size(); i++)
    {
        count *= (source_classes[i] > 0);
    }
#else
    int count = 1;
    int all_sum = 0;
    int partial_sum = 0;
    for(unsigned int i = 0; i < source_classes.size(); i++)
    {
        count *= (source_classes[i] > 0);
        all_sum += _num_samples[i];
        partial_sum += (source_classes[i] > 0)*_num_samples[i];
    }
    
    const float weight = (partial_sum > 0 ? all_sum*1.0/partial_sum : 0);
#endif

    return weight*(count == 1);
}
