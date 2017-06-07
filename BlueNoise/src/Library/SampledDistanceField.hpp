/*
  SampledDistanceField.hpp

  Li-Yi Wei
  07/15/2007

*/

#ifndef _SAMPLED_DISTANCE_FIELD
#define _SAMPLED_DISTANCE_FIELD

#include "DistanceField.hpp"
#include "Array.hpp"

class SampledDistanceField : public DistanceField
{
public:
    SampledDistanceField(const vector<float> & range_min, const vector<float> & range_max, const float average_min_distance, const Array<float> & importance);
    ~SampledDistanceField(void);

    float Query(const Coordinate & sample) const;

    float QueryMax(const Coordinate & region_min, const Coordinate & region_max) const;

    Boundary BoundaryCondition(const int which_dimension) const;
    
protected:
    Array<float> _data;
};

#endif
