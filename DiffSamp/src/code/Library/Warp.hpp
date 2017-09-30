/*
  Warp.hpp

  the base class of all warp

  Li-Yi Wei
  10/15/2010

*/

#ifndef _WARP_HPP
#define _WARP_HPP

#include "Sample.hpp"
#include "Matrix.hpp"

class Warp
{
public:
    virtual ~Warp(void) = 0;

    // the terms forward/inverse came from Wolberg's digital image warping book

    // map/warp source into target
    // return 1 if successful, 0 else
    virtual int ForwardMap(const Coordinate & source, Coordinate & target) const;
    virtual int InverseMap(const Coordinate & source, Coordinate & target) const;

    // get forward/inverse Jacobian
    // reverse the convention in "Anisotropic Blue Noise Sampling" SIGA 2010
    // i.e. use inverse jacobian for most computations!
    // so that query coordinate below would match the source coordinate above
    // note: query is on the *forward* space always!
    virtual int ForwardJacobian(const Coordinate & query, Matrix & jacobian) const;
    virtual int InverseJacobian(const Coordinate & query, Matrix & jacobian) const;
};

#endif
