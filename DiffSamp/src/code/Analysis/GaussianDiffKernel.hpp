/*
  GaussianDiffKernel.hpp

  Li-Yi Wei
  09/05/2010

*/

#ifndef _GAUSSIAN_DIFF_KERNEL_HPP
#define _GAUSSIAN_DIFF_KERNEL_HPP

#include "DiffKernel.hpp"

class GaussianDiffKernel : public DiffKernel
{
public:
    // specify the radius in terms of how many std, usually 3
    GaussianDiffKernel(const float radius_in_std);
    virtual ~GaussianDiffKernel(void);

    // get the distance beyond which the kernel has negligibly small value
    virtual float GetRadius(void) const;

    virtual float Get(const Coordinate & center, const Coordinate & query) const;

protected:
    const float _radius_in_std;
};

#endif
