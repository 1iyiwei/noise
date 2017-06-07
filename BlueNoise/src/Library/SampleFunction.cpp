/*
  SampleFunction.cpp

  Li-Yi Wei
  09/11/2007
  
*/

#include "SampleFunction.hpp"
#include "Exception.hpp"

SampleFunction::Value::Value(void) : value(0), weight(0)
{
    // nothing to do
}

SampleFunction::Value::Value(const float value_in, const float weight_in) : value(value_in), weight(weight_in)
{
    // nothing to do
}

SampleFunction::SampleFunction(void)
{
    // nothing to do
}

SampleFunction::~SampleFunction(void)
{
    // nothing to do
}

SampleFunction::Value SampleFunction::Evaluate(const Coordinate & location) const
{
    throw Exception("SampleFunction::Evaluate() should not be called");

    return Value(0, 0);
}
