/*
 DistanceField.hpp

 the base class for providing minimum distance function
 
 Li-Yi Wei
 07/12/2007
 modified 07/11/2008

*/

#ifndef _DISTANCE_FIELD_HPP
#define _DISTANCE_FIELD_HPP

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
    // implicitly assuming that the sample domain starts with 0
    // (if not, coordinates can be easily shifted as a pre/post-process)
    DistanceField(const vector<float> & domain_size);
    virtual ~DistanceField(void) = 0;

    int Dimension(void) const;
    virtual int NumClasses(void) const; // number of classes this DistanceField would support

    const vector<float> & Domain(void) const;

    // return 1 if inside this distance field, 0 else
    int Inside(const Sample & sample) const;
    
    // return the distance field value for a sample location
    virtual float Query(const Sample & sample) const;

    virtual Boundary BoundaryCondition(const int which_dimension) const;

    virtual int IsUniform(void) const;
    
protected:
    const vector<float> _domain_size;
};

#endif
