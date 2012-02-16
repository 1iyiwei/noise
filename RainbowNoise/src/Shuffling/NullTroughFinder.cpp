/*
  NullTroughFinder.cpp

  Li-Yi Wei
  08/11/2009

*/

#include "NullTroughFinder.hpp"
#include "Exception.hpp"

NullTroughFinder::NullTroughFinder(void)
{
    // nothing to do
}

NullTroughFinder::~NullTroughFinder(void)
{
    // nothing to do
}

int NullTroughFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    throw Exception("NullTroughFinder::Find() shouldn't be called");
    return 0;
}
