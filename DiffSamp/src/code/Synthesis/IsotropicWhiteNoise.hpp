/*
  IsotropicWhiteNoise.hpp

  Li-Yi Wei
  09/01/2010

*/

#ifndef _ADAPTIVE_WHITE_NOISE_HPP
#define _ADAPTIVE_WHITE_NOISE_HPP

#include "WhiteNoise.hpp"
#include "Array.hpp"
#include "UniformWhiteNoise.hpp"

class IsotropicWhiteNoise : public WhiteNoise
{
public:
    IsotropicWhiteNoise(const vector<float> & domain_size, const Array<float> & importance);
    virtual ~IsotropicWhiteNoise(void);

    int Get(Sample & output) const;

protected:
    Array<float> _importance;
    vector<int> _importance_size;
    float _importance_max;
    UniformWhiteNoise _uniform_white_noise;
};

#endif
