/*
  RampDistanceField.cpp

  Li-Yi Wei
  07/16/2007

*/

#include "RampDistanceField.hpp"
#include "Exception.hpp"
#include "Math.hpp"

RampDistanceField::RampDistanceField(const vector<float> & range_min, const vector<float> & range_max, const float average_distance) : DistanceField(range_min, range_max), _average_distance(average_distance)
{
    // nothing to do
}

RampDistanceField::~RampDistanceField(void)
{
    // nothing to do
}

float RampDistanceField::Query(const Coordinate & sample) const
{
    if(sample.Dimension() <= 0)
    {
        return -1;
    }
    else
    {
        const float distance = (sample[0]-_range_min[0])/(_range_max[0]-_range_min[0])*_average_distance*2;

        const int max_num_samples = Math::ComputeMaxNumSamples(_range_min.size(), _average_distance);

        const int local_num_samples = max_num_samples/_average_distance*distance;

        const float local_min_distance = Math::ComputeMaxMinDistance(_range_min.size(), local_num_samples);

        return local_min_distance;
    }
}

float RampDistanceField::QueryMax(const Coordinate & region_min,
                                  const Coordinate & region_max) const
{
    throw Exception("RampDistanceField::QueryMax() not yet done but probably not needed anyway");
    return _average_distance;
}

RampDistanceField::Boundary RampDistanceField::BoundaryCondition(const int which_dimension) const
{
    return BOUNDARY_NONE;
}
