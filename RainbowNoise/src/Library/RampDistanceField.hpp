/*
  RampDistanceField.hpp

  Li-Yi Wei
  07/16/2007
  08/26/2008 : modified for multi-class sampling
*/

#ifndef _RAMP_DISTANCE_FIELD_HPP
#define _RAMP_DISTANCE_FIELD_HPP

#include "DistanceField.hpp"

class RampDistanceField : public DistanceField
{
public: 
    RampDistanceField(const vector<float> & domain_size, const vector<float> & average_distances);
    ~RampDistanceField(void);

    int NumClasses(void) const; 

    float Query(const Sample & sample) const;

    Boundary BoundaryCondition(const int which_dimension) const;

    int IsUniform(void) const;
    
protected:
    const vector<float> _average_distances;
};

#endif
