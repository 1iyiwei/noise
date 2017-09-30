/*
  CosImportanceField.cpp

  Li-Yi Wei
  10/11/2010

*/

#include <math.h>

#include "CosImportanceField.hpp"
#include "Math.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

CosImportanceField::CosImportanceField(const Coordinate & frequency) : _frequency(frequency)
{
    // nothing else to do
}

CosImportanceField::~CosImportanceField(void)
{
    // nothing else to do
}

int CosImportanceField::Dimension(void) const
{
    return _frequency.Dimension();
}

float CosImportanceField::Query(const Sample & sample) const
{
    if(sample.coordinate.Dimension() != _frequency.Dimension()) throw Exception("CosImportanceField::Query(): dimension mismatch");

    const float dot = Utility::Dot(sample.coordinate, _frequency);

    return (cos(2*Math::PI*dot) + 1);
}
