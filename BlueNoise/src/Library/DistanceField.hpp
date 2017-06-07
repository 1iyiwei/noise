/*
 DistanceField.hpp

 the base class for providing minimum distance function
 
 Li-Yi Wei
 07/12/2007

*/

#ifndef _DISTANCE_FIELD_HPP
#define _DISTANCE_FIELD_HPP

#include "Coordinate.hpp"
#include "Sample.hpp"

class DistanceField
{
public:
    // only these two are provided as other tricks
    // such as reflective and repeat ought to be achieved
    // via boundary_none + proper distance field content padding
    // and crop out the central region of the final samples
    typedef enum{BOUNDARY_NONE, BOUNDARY_TOROIDAL} Boundary;
    
public:
    DistanceField(const vector<float> & range_min, const vector<float> & range_max);
    virtual ~DistanceField(void) = 0;

    void GetRange(vector<float> & range_min, vector<float> & range_max) const;

    // return 1 if inside this distance field, 0 else
    int Inside(const Coordinate & coordinate) const;
    
    // return the distance field value for a sample location
    virtual float Query(const Coordinate & sample) const;

    // return the maximum distance field value within a region
    virtual float QueryMax(const Coordinate & region_min, const Coordinate & region_max) const;

    // return 1 if the distance value is legal, 0 else
    int IsLegal(const float distance_value) const;

    virtual Boundary BoundaryCondition(const int which_dimension) const;

    virtual int IsUniform(void) const;
    
protected:
    const vector<float> _range_min;
    const vector<float> _range_max;
};

#endif
