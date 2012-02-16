/*
  DartShuffler.hpp

  the base class for all kinds of dart shuffler

  Li-Yi Wei
  06/11/2009

*/

#ifndef _DART_SHUFFLER_HPP
#define _DART_SHUFFLER_HPP

#include "Sample.hpp"
#include "EnergySample.hpp"

#include "EnergyField.hpp"
#include "PeakFinder.hpp"
#include "TroughFinder.hpp"

class DartShuffler
{
public:
    DartShuffler(EnergyField & energy_field, const PeakFinder & peak_finder, const TroughFinder & trough_finder);
    virtual ~DartShuffler(void) = 0;

    // initialization
    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    // get the sample set
    virtual int Get(vector<Sample> & samples) const;
    virtual int Get(vector<EnergySample> & samples) const;

    // next iteration
    // return peak energy, NAN if error
    virtual float Next(void);

    // report
    virtual float PeakEnergy(void) const;

    // set various flags
    void RemovePeakBeforeFindTrough(const bool preference);
    void UndoIfEnergyIncrease(const bool preference);

protected:
    // proceed with a given a peak
    virtual float Next(EnergySample & peak);

protected:
    // assuming both already belong to the energy sample field
    // return energy value change
    float Swap(EnergySample & s0, EnergySample & s1);
    float Swap(EnergySample & s0, Sample & s1);

    // wrapper for the above 2
    float Swap(EnergySample & s0, EnergySample * ps1, Sample & s1); 

    // return the peak energy decrease after undo
    float Undo(const float old_peak_energy, EnergySample & peak, EnergySample * energy_trough, Sample & sample_trough);

protected:
    EnergyField & _energy_field;
    const PeakFinder & _peak_finder;
    const TroughFinder & _trough_finder;

    bool _remove_peak_before_find_trough;
    bool _undo_if_energy_increase;

    mutable const EnergySample * _p_energy_peak;
    mutable const EnergySample * _p_energy_trough;

    mutable EnergySample _energy_trough;

    mutable Sample _peak, _trough;

    mutable Sample _s0, _s1;
};

#endif
