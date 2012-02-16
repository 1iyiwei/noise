/*
  UniformDistanceField.hpp

  Li-Yi Wei
  07/12/2007
  modified 07/11/2008

*/

#ifndef _UNIFORM_DISTANCE_FIELD_HPP
#define _UNIFORM_DISTANCE_FIELD_HPP

#include "DistanceField.hpp"

class UniformDistanceField : public DistanceField
{
public:
    // the size of min_distances specify the number of classes
    UniformDistanceField(const vector<float> & domain_size, const vector<float> & min_distances);
    ~UniformDistanceField(void);

    int NumClasses(void) const; 

    float Query(const Sample & sample) const;

    Boundary BoundaryCondition(const int which_dimension) const;

    int IsUniform(void) const;
    
protected:
    const vector<float> _min_distances;
};

#endif
