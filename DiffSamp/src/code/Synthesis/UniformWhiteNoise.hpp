/*
  UiformWhiteNoise.hpp

  Li-Yi Wei
  09/01/2010

*/

#ifndef _UNIFORM_WHITE_NOISE_HPP
#define _UNIFORM_WHITE_NOISE_HPP

#include "WhiteNoise.hpp"

class UniformWhiteNoise : public WhiteNoise
{
public:
    UniformWhiteNoise(const vector<float> & domain_size);
    virtual ~UniformWhiteNoise(void);

    int Get(Sample & output) const;
    int Set(const vector<float> & domain_size);
};

#endif
