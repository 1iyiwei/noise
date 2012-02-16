/*
  UniformFairKernelSize.hpp

  sample kernel size for uniform sampling

  Li-Yi Wei
  06/09/2009

*/

#ifndef _UNIFORM_FAIR_KERNEL_SIZE_HPP
#define _UNIFORM_FAIR_KERNEL_SIZE_HPP

#include "UniformKernelSize.hpp"

class UniformFairKernelSize : public UniformKernelSize
{
public:
    UniformFairKernelSize(const vector<float> & domain_size, const vector<int> & num_samples_per_class);

protected:
    virtual float GetSize(const Sample & sample, const vector<int> & source_classes) const;
    virtual float GetSize(const Sample & sample, const int class_id0, const int class_id1) const;
};

#endif
