/*
  UniformKernelSize.hpp

  sample kernel size for uniform sampling

  Li-Yi Wei
  06/09/2009

*/

#ifndef _UNIFORM_KERNEL_SIZE_HPP
#define _UNIFORM_KERNEL_SIZE_HPP

#include <vector>
using namespace std;

#include "KernelSize.hpp"

class UniformKernelSize : public KernelSize
{
public:
    UniformKernelSize(const vector<float> & domain_size, const vector<int> & num_samples_per_class);
    virtual ~UniformKernelSize(void) = 0;

    int NumClass(void) const;

protected:
    virtual float GetSize(const Sample & sample, const vector<int> & source_classes) const;
    virtual float GetSize(const Sample & sample, const int class_id0, const int class_id1) const;

protected:
    static float Volume(const vector<float> & domain_size);
    const vector<float> _domain_size;
    const float _domain_volume;
    const vector<int> _num_samples;
};

#endif
