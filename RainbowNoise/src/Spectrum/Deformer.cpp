/*
  Deformer.cpp

  Li-Yi Wei
  08/26/2008

*/

#include "Deformer.hpp"
#include "Exception.hpp"

Deformer::Deformer(void) 
{
    // nothing to do
}

Deformer::~Deformer(void)
{
    // nothing to do
}

float Deformer::Query(const Sample & s1, const Sample & s2) const
{
    // should not be called
    throw Exception("Deformer::Query() shouldn't be called");

    return 1; // default, non-deformed value
}
