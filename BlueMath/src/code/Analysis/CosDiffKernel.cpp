/*
  CosDiffKernel.cpp

  Li-Yi Wei
  10/06/2010

*/

#include <math.h>

#include "CosDiffKernel.hpp"
#include "Math.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

CosDiffKernel::CosDiffKernel(void): _offset(0)
{
    // nothing else to do
}

CosDiffKernel::CosDiffKernel(const float offset): _offset(offset)
{
    // nothing else to do
}

CosDiffKernel::~CosDiffKernel(void)
{
    // nothing else to do
}

float CosDiffKernel::GetRadius(void) const
{
    return Math::INF;
}

float CosDiffKernel::Get(const Coordinate & center, const Coordinate & query) const
{
    return cos(2*Math::PI*Utility::Dot(center, query)) + _offset;
}
