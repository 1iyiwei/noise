/*
 DistanceField.hpp

 the base class for providing minimum distance function
 
 Li-Yi Wei
 07/12/2007

 07/11/2008
 modified for multi-class

 09/14/2010
 modify back for single-class

*/

#ifndef _DISTANCE_FIELD_HPP
#define _DISTANCE_FIELD_HPP

#include "Sample.hpp"
#include "MetricField.hpp"

class DistanceField : public MetricField
{
public:
    // implicitly assuming that the sample domain starts with 0
    // (if not, coordinates can be easily shifted as a pre/post-process)
    DistanceField(const Domain & domain);
    virtual ~DistanceField(void) = 0;

    // return the distance field value for a sample location
    virtual float Query(const Sample & sample) const;

    // return the mean distance
    virtual float Mean(void) const;

    // max distance
    virtual float Max(void) const;
};

#endif
