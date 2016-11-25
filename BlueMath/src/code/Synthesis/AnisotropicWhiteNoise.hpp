/*
  AnisotropicWhiteNoise.hpp

  Li-Yi Wei
  10/16/2010

*/

#ifndef _ANISOTROPIC_WHITE_NOISE_HPP
#define _ANISOTROPIC_WHITE_NOISE_HPP

#include "WhiteNoise.hpp"
#include "JacobianField.hpp"
#include "UniformWhiteNoise.hpp"

class AnisotropicWhiteNoise : public WhiteNoise
{
public:
    // radius is the little radius for domain sampling sphere
    // see Anisotropic Blue Noise Sampling in SIGGRAPH Asia 2010
    AnisotropicWhiteNoise(const JacobianField & jacobian_field, const float radius);
    virtual ~AnisotropicWhiteNoise(void);

    int Get(Sample & output) const;

protected:
    const JacobianField & _jacobian_field;
    const float _radius;
    const float _inverse_bounding_radius;

    UniformWhiteNoise _uniform_white_noise;
    UniformWhiteNoise _unit_white_noise;
};

#endif
