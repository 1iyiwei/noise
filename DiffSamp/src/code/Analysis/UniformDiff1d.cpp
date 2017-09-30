/*
  UniformDiff1d.cpp

  Li-Yi Wei
  04/22/2010

*/

#include "UniformDiff1d.hpp"
#include "Random.hpp"

UniformDiff1d::UniformDiff1d(const float min, const float max) : _min(min), _max(max)
{
    // nothing else to do
}

UniformDiff1d::~UniformDiff1d(void)
{
    // nothing else to do
}

float UniformDiff1d::Diff(void) const
{
    return (_min + Random::UniformRandom()*(_max-_min));
}

void UniformDiff1d::Set(const float min, const float max)
{
    _min = min;
    _max = max;
}
