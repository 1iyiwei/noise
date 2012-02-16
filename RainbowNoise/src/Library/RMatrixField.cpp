/*
  RMatrixField.cpp

  Li-Yi Wei
  09/11/2008

*/

#include "RMatrixField.hpp"
#include "Exception.hpp"

RMatrixField::RMatrixField(void)
{
    // nothing to do
}

RMatrixField::~RMatrixField(void)
{
    // nothing to do
}

const Array<float> & RMatrixField::Query(const Sample & sample) const
{
    // should not be called
    throw Exception("RMatrixField::Query() should not be called");
    return _foo;
}

float RMatrixField::MaxDistance(void) const
{
    // should not be called
    throw Exception("RMatrixField::MaxDistance() should not be called");
    return 0;
}
