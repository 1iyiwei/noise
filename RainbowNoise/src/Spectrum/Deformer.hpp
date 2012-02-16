/*
  Deformer.hpp

  the base virtual class for the deformation term

  08/26/2008

*/

#ifndef _DEFORMER_HPP
#define _DEFORMER_HPP

#include "Sample.hpp"

class Deformer
{
public:
    Deformer(void);
    virtual ~Deformer(void) = 0;

    virtual float Query(const Sample & s1, const Sample & s2) const;
};

#endif
