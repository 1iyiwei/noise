/*
  DistanceField.cpp

  Li-Yi Wei
  07/12/2007

*/

#include "DistanceField.hpp"
#include "Exception.hpp"

DistanceField::DistanceField(const vector<float> & range_min, const vector<float> & range_max) : _range_min(range_min), _range_max(range_max)
{
    if(_range_min.size() != _range_max.size())
    {
        throw Exception("DistanceField::DistanceField(const vector<float> & range_min, const vector<float> & range_max) : range_min.size() != range_max.size()");
    }
}

DistanceField::~DistanceField(void)
{
    // nothing to do
}
   
void DistanceField::GetRange(vector<float> & range_min, vector<float> & range_max) const
{
    range_min = _range_min;
    range_max = _range_max;
}

int DistanceField::Inside(const Coordinate & coordinate) const
{
    if(coordinate.Dimension() != _range_min.size())
    {
        // mismatch
        return 0;
    }

    int inside = 1;

    for(unsigned int i = 0; i < _range_min.size(); i++)
    {
        if((coordinate[i] < _range_min[i]) || (coordinate[i] > _range_max[i]))
        {
            inside = 0;
        }
    }

    return inside;
}
    
float DistanceField::Query(const Coordinate & sample) const
{
    // shouldn't be called
    return -1;
}
   
float DistanceField::QueryMax(const Coordinate & region_min,
                              const Coordinate & region_max) const
{
    // shouldn't be called
    return -1;
}
  
int DistanceField::IsLegal(const float distance_value) const
{
    return (distance_value > 0);
}

DistanceField::Boundary DistanceField::BoundaryCondition(const int which_dimension) const
{
    // shouldn't be called

    return BOUNDARY_NONE;
}

int DistanceField::IsUniform(void) const
{
    return 0;
}
