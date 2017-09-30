/*
  GaussianDiff1d.hpp

  gaussian random diff 1d

  Li-Yi Wei
  04/22/2010

*/

#ifndef _GAUSSIAN_DIFF_1D_HPP
#define _GAUSSIAN_DIFF_1D_HPP

#include "Diff1d.hpp"

class GaussianDiff1d: public Diff1d
{
public:
    GaussianDiff1d(const float min, const float max, const float std);
    virtual ~GaussianDiff1d(void);

    float Diff(void) const;

protected:
    const float _min, _max, _std;
};

#endif
