/*
  Diff1d.hpp

  the base class for all pair-wise 1D sample location diff

  Li-Yi Wei
  04/22/2010

*/

#ifndef _DIFF_1D_HPP
#define _DIFF_1D_HPP

class Diff1d
{
public:
    virtual ~Diff1d(void) = 0;

    virtual float Diff(void) const;
};
#endif
