/*
  RampDistanceField.hpp

  Li-Yi Wei
  07/16/2007

*/

#ifndef _RAMP_DISTANCE_FIELD_HPP
#define _RAMP_DISTANCE_FIELD_HPP

#include "DistanceField.hpp"

class RampDistanceField : public DistanceField
{
public:
    RampDistanceField(const vector<float> & range_min, const vector<float> & range_max, const float average_distance);
    ~RampDistanceField(void);

    float Query(const Coordinate & sample) const;

    float QueryMax(const Coordinate & region_min, const Coordinate & region_max) const;

    Boundary BoundaryCondition(const int which_dimension) const;
    
protected:
    const float _average_distance;
};

#endif
