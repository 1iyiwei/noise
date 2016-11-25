/*
  GaussianDiff1d.cpp

  Li-Yi Wei
  04/22/2010

*/

#include <math.h>

#include "GaussianDiff1d.hpp"
#include "Random.hpp"
#include "Math.hpp"

GaussianDiff1d::GaussianDiff1d(const float min, const float max, const float std) : _min(min), _max(max), _std(std)
{
    // nothing else to do
}

GaussianDiff1d::~GaussianDiff1d(void)
{
    // nothing else to do
}

float GaussianDiff1d::Diff(void) const
{
    float value = Math::INF;

    while(value > _max)
    {
        value = _min + fabs(Random::GaussianRandom()*_std);
    }

    return value;
}
