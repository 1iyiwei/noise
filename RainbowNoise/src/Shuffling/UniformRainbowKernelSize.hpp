/*
  UniformRainbowKernelSize.hpp

  construct from RMatrix

  Li-Yi Wei
  08/29/2009

*/

#ifndef _UNIFORM_RAINBOW_KERNEL_SIZE_HPP
#define _UNIFORM_RAINBOW_KERNEL_SIZE_HPP

#include "UniformKernelSize.hpp"
#include "RMatrix.hpp"
#include "Array.hpp"

class UniformRainbowKernelSize : public UniformKernelSize
{
public:
    UniformRainbowKernelSize(const vector<float> & domain_size, const RMatrix::Method method, const vector<int> & num_samples_per_class);

protected:
    virtual float GetSize(const Sample & sample, const vector<int> & source_classes) const;
    virtual float GetSize(const Sample & sample, const int class_id0, const int class_id1) const;

protected:
    const Array<float> _r_matrix;
    mutable vector<int> _index;
};

#endif
