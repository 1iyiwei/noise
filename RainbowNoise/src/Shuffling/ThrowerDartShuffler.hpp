/*
  ThrowerDartShuffler.hpp

  soft dart throwing for initializing samples

  Li-Yi Wei
  09/07/2009

*/

#ifndef _THROWER_DART_SHUFFLER_HPP
#define _THROWER_DART_SHUFFLER_HPP

#include "DartShuffler.hpp"

class ThrowerDartShuffler : public DartShuffler
{
public:
    ThrowerDartShuffler(EnergyField & energy_field, const PeakFinder & peak_finder, const TroughFinder & trough_finder);
    virtual ~ThrowerDartShuffler(void);

    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    virtual float Next(void);

protected:
    int LocalSet(void);

protected:
    vector<EnergySample *> _samples_remaining;
};

#endif
