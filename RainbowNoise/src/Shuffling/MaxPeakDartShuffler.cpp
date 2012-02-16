/*
  MaxPeakDartShuffler.cpp

  Li-Yi Wei
  08/26/2009

*/

#include "MaxPeakDartShuffler.hpp"

MaxPeakDartShuffler::MaxPeakDartShuffler(EnergyField & energy_field, const PeakFinder & peak_finder, const TroughFinder & trough_finder) : DartShuffler(energy_field, peak_finder, trough_finder)
{
    // nothing else to do
}

MaxPeakDartShuffler::~MaxPeakDartShuffler(void)
{
    // nothing else to do
}

float MaxPeakDartShuffler::Next(void)
{
    return DartShuffler::Next();
}
