/*
  UniformDistanceField.hpp

  Li-Yi Wei
  07/12/2007

*/

#ifndef _UNIFORM_DISTANCE_FIELD_HPP
#define _UNIFORM_DISTANCE_FIELD_HPP

#include "DistanceField.hpp"

class UniformDistanceField : public DistanceField
{
public:
    UniformDistanceField(const vector<float> & range_min, const vector<float> & range_max, const float min_distance);
    ~UniformDistanceField(void);

    float Query(const Coordinate & sample) const;

    float QueryMax(const Coordinate & region_min, const Coordinate & region_max) const;

    Boundary BoundaryCondition(const int which_dimension) const;

    int IsUniform(void) const;
    
protected:
    const float _min_distance;
};

#endif
