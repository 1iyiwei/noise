/*
  Uniform Analytical Power Fourier Transform

  Li-Yi Wei
  08/30/2010

*/

#ifndef _UNIFORM_AFT_HPP
#define _UNIFORM_AFT_HPP

#include "AFT.hpp"

class UniformAFT : public AFT
{
public:
    UniformAFT(const int num_samples, const float normalization_const);
    virtual ~UniformAFT(void);

    virtual float Evaluate(const Coordinate & w) const;
};

#endif

