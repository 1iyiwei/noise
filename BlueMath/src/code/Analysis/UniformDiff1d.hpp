/*
  UniformDiff1d.hpp

  uniform random diff 1d

  Li-Yi Wei
  04/22/2010

*/

#ifndef _UNIFORM_DIFF_1D_HPP
#define _UNIFORM_DIFF_1D_HPP

#include "Diff1d.hpp"

class UniformDiff1d: public Diff1d
{
public:
    UniformDiff1d(const float min, const float max);
    virtual ~UniformDiff1d(void);

    float Diff(void) const;

protected:
    void Set(const float min, const float max);

protected:
    float _min, _max;
};

#endif
