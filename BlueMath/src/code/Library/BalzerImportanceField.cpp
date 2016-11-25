/*
  BalzerImportanceField.cpp

  Li-Yi Wei
  10/11/2010

*/

#include <math.h>

#include "BalzerImportanceField.hpp"
#include "Math.hpp"

BalzerImportanceField::BalzerImportanceField(const int dimension, const float exp_value, const float sin_weight, const float sin_freq) : _dimension(dimension), _exp_value(exp_value), _sin_weight(sin_weight), _sin_freq(sin_freq)
{
    // nothing else to do
}

BalzerImportanceField::~BalzerImportanceField(void)
{
    // nothing else to do
}

int BalzerImportanceField::Dimension(void) const
{
    return _dimension;
}

float BalzerImportanceField::Query(const Sample & sample) const
{
    float exp_part = 0;

    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        exp_part += sample.coordinate[i]*sample.coordinate[i];
    }

    exp_part = exp(_exp_value*exp_part);

    float sin_part = _sin_weight;

    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        const float term = sin(Math::PI*_sin_freq*sample.coordinate[i]);

        sin_part *= term*term;
    }

    return (exp_part + sin_part);
}
