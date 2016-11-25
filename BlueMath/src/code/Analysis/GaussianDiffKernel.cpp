/*
  GaussianDiffKernel.cpp

  Li-Yi Wei
  09/05/2010

*/

#include <math.h>

#include "GaussianDiffKernel.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

GaussianDiffKernel::GaussianDiffKernel(const float radius_in_std): _radius_in_std(radius_in_std)
{
    // nothing else to do
}

GaussianDiffKernel::~GaussianDiffKernel(void)
{
    // nothing else to do
}

float GaussianDiffKernel::GetRadius(void) const
{
    return _size*_radius_in_std;
}

float GaussianDiffKernel::Get(const Coordinate & center, const Coordinate & query) const
{
    const float distance2 = Utility::Distance2(center, query);

    return exp(-distance2/(_size*_size));
}
