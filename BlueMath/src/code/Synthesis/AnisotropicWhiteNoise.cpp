/*
  AnisotropicWhiteNoise.cpp

  Li-Yi Wei
  10/16/2010

*/

#include "AnisotropicWhiteNoise.hpp"
#include "Random.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

AnisotropicWhiteNoise::AnisotropicWhiteNoise(const JacobianField & jacobian_field, const float radius): WhiteNoise(jacobian_field.BoundingBox()), _jacobian_field(jacobian_field), _radius(radius), _inverse_bounding_radius(jacobian_field.InverseMaxEigenValue()), _uniform_white_noise(jacobian_field.BoundingBox()), _unit_white_noise(vector<float>(jacobian_field.Dimension(), 1))
{
    if(radius <= 0)
    {
        throw Exception("AnisotropicWhiteNoise::AnisotropicWhiteNoise(): radius must > 0");
    }
}

AnisotropicWhiteNoise::~AnisotropicWhiteNoise(void)
{
    // nothing else to do
}

int AnisotropicWhiteNoise::Get(Sample & sample) const
{
    /*static*/ Sample anchor, inverse_sample, forward_sample;
    /*static*/ Matrix forward_jacobian, inverse_jacobian;

    do
    {
        // uniform white anchor
        if(! _uniform_white_noise.Get(anchor))
        {
            throw Exception("AnisotropicWhiteNoise::Get(): cannot get uniform anchor sample");
            return 0;
        }

        if(! _jacobian_field.Forward(anchor, forward_jacobian))
        {
            throw Exception("AnisotropicWhiteNoise::Get(): cannot get forward jacobian");
            return 0;
        }
        // if(! _jacobian_field.Inverse(anchor, inverse_jacobian)) return 0;

        // random inverse sample around (inverse) anchor
        if(! _unit_white_noise.Get(inverse_sample)) return 0;
        for(int i = 0; i < inverse_sample.coordinate.Dimension(); i++)
        {
            inverse_sample.coordinate[i] -= 0.5;
            inverse_sample.coordinate[i] *= 2*_inverse_bounding_radius;
        }

        if(! forward_jacobian.Multiply(inverse_sample.coordinate, forward_sample.coordinate))
        {
            throw Exception("AnisotropicWhiteNoise::Get(): cannot forward map sample");
            return 0;
        }

        sample.coordinate = Utility::Add(anchor.coordinate, Utility::Multiply(forward_sample.coordinate, _radius));
    }
    while((Utility::Norm2(forward_sample.coordinate) > 1.0) || !_jacobian_field.Inside(sample));

    sample.id = 0;
    sample.value = 1.0;

    // done
    return 1;
}
