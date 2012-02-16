/*
  SampleFunction.hpp
  base class for all demo sample functions
  
  Li-Yi Wei
  09/11/2007

*/

#ifndef _SAMPLE_FUNCTION_HPP
#define _SAMPLE_FUNCTION_HPP

#include "Sample.hpp"

class SampleFunction
{
public:
    struct Value
    {
        Value(void);
        Value(const float value, const float weight);
        
        float value;
        float weight;
    };
    
    SampleFunction(void);
    virtual ~SampleFunction(void) = 0;

    // zero weight indicates location is outside
    // negative weight in value indicates illegal input (e.g. wrong dimension)
    virtual Value Evaluate(const Coordinate & location) const;
};

#endif
