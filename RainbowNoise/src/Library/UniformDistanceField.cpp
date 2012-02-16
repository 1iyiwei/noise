/*
  UniformDistanceField.cpp

  Li-Yi Wei
  07/12/2007

*/

#include "UniformDistanceField.hpp"
#include "Exception.hpp"

UniformDistanceField::UniformDistanceField(const vector<float> & domain_size, const vector<float> & min_distances) : DistanceField(domain_size), _min_distances(min_distances)
{
    // nothing to do
}

UniformDistanceField::~UniformDistanceField(void)
{
    // nothing to do
}

int UniformDistanceField::NumClasses(void) const
{
    return _min_distances.size();
}

float UniformDistanceField::Query(const Sample & sample) const
{
    if((sample.id >= 0) && (sample.id < _min_distances.size()))
    {
        return _min_distances[sample.id];
    }
    else
    {
        throw Exception("UniformDistanceField::Query() : illegal sample class id");
        return -1;
    }
}

UniformDistanceField::Boundary UniformDistanceField::BoundaryCondition(const int which_dimension) const
{
    return BOUNDARY_TOROIDAL;
}

int UniformDistanceField::IsUniform(void) const
{
    return 1;
}
