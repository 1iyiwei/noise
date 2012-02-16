/*
  DartSwapper.cpp

  Li-Yi Wei
  08/17/2009

*/

#include "DartSwapper.hpp"
#include "Math.hpp"
#include "Exception.hpp"

DartSwapper::DartSwapper(EnergyField & energy_field) : _energy_field(energy_field)
{
    // nothing else to do
}

DartSwapper::~DartSwapper(void)
{
    // nothing else to do
}

int DartSwapper::Set(const vector<Sample> & samples)
{
    return _energy_field.Set(samples);
}

int DartSwapper::Get(vector<Sample> & samples) const
{
    return _energy_field.Get(samples);
}

float DartSwapper::Next(void)
{
    throw Exception("DartSwapper::Next(): shouldn't be called");
    return Math::NAN;
}
