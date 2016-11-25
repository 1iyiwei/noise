/*
  UniformDistanceField.hpp

  Li-Yi Wei
  07/12/2007
  
  07/11/2008
  modified for multi-class

  09/14/2010
  modify back for single-class

*/

#ifndef _UNIFORM_DISTANCE_FIELD_HPP
#define _UNIFORM_DISTANCE_FIELD_HPP

#include "DistanceField.hpp"

class UniformDistanceField : public DistanceField
{
public:
    // min_distance is the constant distance value
    UniformDistanceField(const Domain & domain, const float min_distances);
    ~UniformDistanceField(void);

    float Query(const Sample & sample) const;

    float Mean(void) const;

    float Max(void) const;

protected:
    const float _min_distance;
};

#endif
