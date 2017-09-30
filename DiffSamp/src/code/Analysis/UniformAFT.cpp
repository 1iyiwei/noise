/*
  UniformAFT.cpp

  Li-Yi Wei
  08/30/2010

*/

#include "UniformAFT.hpp"

UniformAFT::UniformAFT(const int num_samples, const float normalization_const) : AFT(num_samples, normalization_const)
{
    // nothing else to do
}

UniformAFT::~UniformAFT(void)
{
    // nothing else to do
}

float UniformAFT::Evaluate(const Coordinate & w) const
{
    int is_dc = 1;

    for(int i = 0; i < w.Dimension(); i++)
    {
        is_dc *= (w[i] == 0);
    }

    const float non_dc_power = _num_samples * _normalization_const * _normalization_const;
    const float dc_power = _num_samples * non_dc_power;

    const float value = (is_dc ? dc_power : non_dc_power);

    return value;
}
