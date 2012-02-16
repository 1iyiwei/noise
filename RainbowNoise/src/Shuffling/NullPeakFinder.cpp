/*
  NullPeakFinder.cpp

  Li-Yi Wei
  06/18/2009

*/

#include "NullPeakFinder.hpp"
#include "Exception.hpp"

NullPeakFinder::NullPeakFinder(void)
{
    // nothing to do
}

NullPeakFinder::~NullPeakFinder(void)
{
    // nothing to do
}

const EnergySample * NullPeakFinder::Find(const EnergyField & energy_field) const
{
    return 0;
}

int NullPeakFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    return 0;
}
