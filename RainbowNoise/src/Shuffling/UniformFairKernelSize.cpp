/*
  UniformFairKernelSize.cpp

  Li-Yi Wei
  06/09/2009

*/

#include "UniformFairKernelSize.hpp"
#include "Math.hpp"

UniformFairKernelSize::UniformFairKernelSize(const vector<float> & domain_size, const vector<int> & num_samples_per_class) : UniformKernelSize(domain_size, num_samples_per_class) 
{
    // nothing else to do
}

float UniformFairKernelSize::GetSize(const Sample & sample, const vector<int> & source_classes) const
{
    if(source_classes.size() != _num_samples.size())
    {
        return 0;
    }

    int partial_sum = 0;

    for(unsigned int i = 0; i < source_classes.size(); i++)
    {
        partial_sum += (source_classes[i] > 0)*_num_samples[i];
    }

    return (partial_sum > 0 ? Math::ComputeMaxMinDistance(_domain_size.size(), partial_sum/_domain_volume)*0.5 : 0);
    // return (partial_sum > 0 ? Math::HyperSphereRadius(_domain_size.size(), _domain_volume/partial_sum) : 0);
}

float UniformFairKernelSize::GetSize(const Sample & sample, const int class_id0, const int class_id1) const
{
    int partial_sum = 0;

    if((0 <= class_id0) && (class_id0 < _num_samples.size()))
    {
        partial_sum += _num_samples[class_id0];
    }

    if((0 <= class_id1) && (class_id1 < _num_samples.size()) && (class_id1 != class_id0))
    {
        partial_sum += _num_samples[class_id1];
    } 

    return (partial_sum > 0 ? Math::ComputeMaxMinDistance(_domain_size.size(), partial_sum/_domain_volume)*0.5 : 0);
}
