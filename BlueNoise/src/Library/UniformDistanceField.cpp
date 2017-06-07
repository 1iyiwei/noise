/*
  UniformDistanceField.cpp

  Li-Yi Wei
  07/12/2007

*/

#include "UniformDistanceField.hpp"

UniformDistanceField::UniformDistanceField(const vector<float> & range_min, const vector<float> & range_max, const float min_distance) : DistanceField(range_min, range_max), _min_distance(min_distance)
{
    // nothing to do
}

UniformDistanceField::~UniformDistanceField(void)
{
    // nothing to do
}

float UniformDistanceField::Query(const Coordinate & sample) const
{
    return _min_distance;
}

float UniformDistanceField::QueryMax(const Coordinate & region_min,
                                     const Coordinate & region_max) const
{
    return _min_distance;
}

UniformDistanceField::Boundary UniformDistanceField::BoundaryCondition(const int which_dimension) const
{
    return BOUNDARY_TOROIDAL;
}

int UniformDistanceField::IsUniform(void) const
{
    return 1;
}
