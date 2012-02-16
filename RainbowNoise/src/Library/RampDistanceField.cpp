/*
  RampDistanceField.cpp

  Li-Yi Wei
  07/16/2007

*/

#include "RampDistanceField.hpp"
#include "Exception.hpp"
#include "Math.hpp"

RampDistanceField::RampDistanceField(const vector<float> & domain_size, const vector<float> & average_distances) : DistanceField(domain_size), _average_distances(average_distances)
{
    // nothing to do
}

RampDistanceField::~RampDistanceField(void)
{
    // nothing to do
}

int RampDistanceField::NumClasses(void) const
{
    return _average_distances.size();
}

float RampDistanceField::Query(const Sample & sample) const
{
    if((sample.id >= 0) && (sample.id < _average_distances.size()))
    {
        if(sample.coordinate.Dimension() <= 0)
        {
            return -1;
        }
        else
        {
            const float distance = (sample.coordinate[0])/(_domain_size[0])*_average_distances[sample.id]*2;

            const float _average_distance = _average_distances[sample.id];

            const int max_num_samples = Math::ComputeMaxNumSamples(_domain_size.size(), _average_distance);

            const int local_num_samples = max_num_samples/_average_distance*distance;

            const float local_min_distance = Math::ComputeMaxMinDistance(_domain_size.size(), local_num_samples);

            return local_min_distance;
        }
    }
    else
    {
        throw Exception("RampDistanceField::Query() : illegal sample class id");
        return -1;
    }
}

RampDistanceField::Boundary RampDistanceField::BoundaryCondition(const int which_dimension) const
{
    return BOUNDARY_NONE;
}

int RampDistanceField::IsUniform(void) const
{
    return 0;
}
