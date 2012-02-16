/*
  MaxPeakDartShuffler.hpp

  the default dart shuffler that always tries to move/swap the maximum peak

  Li-Yi Wei
  08/26/2009

*/

#ifndef _MAX_PEAK_DART_SHUFFLER_HPP
#define _MAX_PEAK_DART_SHUFFLER_HPP

#include "DartShuffler.hpp"

class MaxPeakDartShuffler : public DartShuffler
{
public:
    MaxPeakDartShuffler(EnergyField & energy_field, const PeakFinder & peak_finder, const TroughFinder & trough_finder);
    virtual ~MaxPeakDartShuffler(void);

    virtual float Next(void);
};

#endif
