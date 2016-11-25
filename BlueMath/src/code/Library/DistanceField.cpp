/*
  DistanceField.cpp

  Li-Yi Wei

  07/11/2008
  modified for multi-class

  09/14/2010
  modify back for single-class

*/

#include "DistanceField.hpp"
#include "Exception.hpp"

DistanceField::DistanceField(const Domain & domain) : MetricField(domain)
{
    // nothing to do
}

DistanceField::~DistanceField(void)
{
    // nothing to do
}
    
float DistanceField::Query(const Sample & sample) const
{
    // shouldn't be called
    throw Exception("DistanceField::Query() should not be called");
    return -1;
}

float DistanceField::Mean(void) const
{
    // shouldn't be called
    throw Exception("DistanceField::Mean() should not be called");
    return -1;
}

float DistanceField::Max(void) const
{
    // shouldn't be called
    throw Exception("DistanceField::Max() should not be called");
    return -1;
}
