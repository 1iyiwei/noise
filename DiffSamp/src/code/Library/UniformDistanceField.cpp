/*
  UniformDistanceField.cpp

  Li-Yi Wei
  07/12/2007

  07/11/2008
  modified for multi-class

  09/14/2010
  modify back for single-class

*/

#include "UniformDistanceField.hpp"
#include "Exception.hpp"

UniformDistanceField::UniformDistanceField(const Domain & domain, const float min_distance) : DistanceField(domain), _min_distance(min_distance)
{
    // nothing to do
}

UniformDistanceField::~UniformDistanceField(void)
{
    // nothing to do
}

float UniformDistanceField::Query(const Sample & sample) const
{
    return _min_distance;
}

float UniformDistanceField::Mean(void) const
{
    return _min_distance;
}

float UniformDistanceField::Max(void) const
{
    return _min_distance;
}
