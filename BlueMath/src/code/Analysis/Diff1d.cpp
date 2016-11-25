/*
  Diff1d.cpp

  Li-Yi Wei
  04/22/2010

*/

#include "Diff1d.hpp"
#include "Exception.hpp"

Diff1d::~Diff1d(void)
{
    // nothing else to do
}

float Diff1d::Diff(void) const
{
    throw Exception("Diff1d::Diff() shouldn't be called!");
    return 0;
}
