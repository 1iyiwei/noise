/*
  DistanceField.cpp

  Li-Yi Wei
  07/12/2007
  modified 07/11/2008

*/

#include "DistanceField.hpp"
#include "Exception.hpp"

DistanceField::DistanceField(const vector<float> & domain_size) : _domain_size(domain_size)
{
    // nothing to do
}

DistanceField::~DistanceField(void)
{
    // nothing to do
}

int DistanceField::Dimension(void) const
{
    return _domain_size.size();
}

int DistanceField::NumClasses(void) const
{
    // should not be called
    return 0;
}

const vector<float> & DistanceField::Domain(void) const
{
    return _domain_size;
}

int DistanceField::Inside(const Sample & sample) const
{
    const Coordinate & coordinate = sample.coordinate;

    if(coordinate.Dimension() != _domain_size.size())
    {
        // mismatch
        return 0;
    }

    int inside = 1;

    for(unsigned int i = 0; i < _domain_size.size(); i++)
    {
        if((coordinate[i] < 0) || (coordinate[i] > _domain_size[i]))
        {
            inside = 0;
        }
    }

    return inside;
}
    
float DistanceField::Query(const Sample & sample) const
{
    // shouldn't be called
    throw Exception("DistanceField::Query() should not be called");
    return -1;
}

DistanceField::Boundary DistanceField::BoundaryCondition(const int which_dimension) const
{
    // shouldn't be called
    throw Exception("DistanceField::BoundaryCondition() should not be called");
    return BOUNDARY_NONE;
}

int DistanceField::IsUniform(void) const
{
    return 0;
}
