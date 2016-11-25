/*
  Analytical Power Fourier Transform

  Li-Yi Wei
  08/30/2010

*/

#ifndef _AFT_HPP
#define _AFT_HPP

#include <vector>
using namespace std;

#include "PFT.hpp"

class AFT : public PFT
{
public:
    AFT(const int num_samples, const float normalization_const);
    virtual ~AFT(void) = 0;

    virtual float Evaluate(const Coordinate & w) const;

protected:
    const int _num_samples;
    const float _normalization_const;
};

#endif

