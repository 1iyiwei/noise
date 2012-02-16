/*
  PeakFinder.cpp

  Li-Yi Wei
  06/10/2009

*/

#include "PeakFinder.hpp"
#include "Exception.hpp"

PeakFinder::~PeakFinder(void)
{
    // nothing to do
}

const EnergySample * PeakFinder::Find(const EnergyField & energy_field) const
{
    throw Exception("PeakFinder::Find() shouldn't be called");
    return 0;
}

int PeakFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    throw Exception("PeakFinder::Find() shouldn't be called");
    return 0;
}
