/*
  TroughFinder.cpp

  Li-Yi Wei
  06/10/2009

*/

#include "TroughFinder.hpp"
#include "Math.hpp"
#include "Exception.hpp"

TroughFinder::TroughFinder(void) : _threshold_energy(0)
{
    // nothing to do
}

TroughFinder::~TroughFinder(void)
{
    // nothing to do
}

int TroughFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    throw Exception("TroughFinder::Find() shouldn't be called");
    return 0;
}

void TroughFinder::ThresholdEnergy(const float threshold_energy)
{
    _threshold_energy = threshold_energy;
}
