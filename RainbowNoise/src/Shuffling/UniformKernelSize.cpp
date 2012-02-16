/*
  UniformKernelSize.cpp

  Li-Yi Wei
  06/09/2009

*/

#include "UniformKernelSize.hpp"
#include "Exception.hpp"

UniformKernelSize::UniformKernelSize(const vector<float> & domain_size, const vector<int> & num_samples_per_class) : _domain_size(domain_size), _domain_volume(Volume(domain_size)), _num_samples(num_samples_per_class)
{
    // nothing else to do
}

UniformKernelSize::~UniformKernelSize(void)
{
    // nothing else to do
}

float UniformKernelSize::GetSize(const Sample & sample, const vector<int> & source_classes) const
{
    throw Exception("UniformKernelSize::GetSize(): shouldn't be called");
    return 0;
}

float UniformKernelSize::GetSize(const Sample & sample, const int class_id0, const int class_id1) const
{
    throw Exception("UniformKernelSize::GetSize(): shouldn't be called");
    return 0;
}

int UniformKernelSize::NumClass(void) const
{
    return _num_samples.size();
}

float UniformKernelSize::Volume(const vector<float> & domain_size)
{
    float volume = (domain_size.size() > 0);

    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        volume *= domain_size[i];
    }

    return volume;
}
