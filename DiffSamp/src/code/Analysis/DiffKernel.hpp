/*
  DiffKernel.hpp

  the base class for all diff splatting kernels

  Li-Yi Wei
  09/05/2010

*/

#ifndef _DIFF_KERNEL_HPP
#define _DIFF_KERNEL_HPP

#include "Sample.hpp"

class DiffKernel
{
public:
    DiffKernel(void);
    virtual ~DiffKernel(void) = 0;

    // size of the kernel, e.g. std of Gaussian
    virtual void SetSize(const float size);

    // get the distance beyond which the kernel has negligibly small value
    virtual float GetRadius(void) const;

    virtual float Get(const Coordinate & center, const Coordinate & query) const;

protected:
    float _size;
};

#endif
