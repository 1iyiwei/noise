/*
  CosDiffKernel.hpp

  Li-Yi Wei
  10/06/2010

*/

#ifndef _COS_DIFF_KERNEL_HPP
#define _COS_DIFF_KERNEL_HPP

#include "DiffKernel.hpp"

class CosDiffKernel : public DiffKernel
{
public:
    CosDiffKernel(void);
    CosDiffKernel(const float offset);
    virtual ~CosDiffKernel(void);

    // get the distance beyond which the kernel has negligibly small value
    virtual float GetRadius(void) const;

    virtual float Get(const Coordinate & center, const Coordinate & query_freq) const;

protected:
    const float _offset;
};

#endif
