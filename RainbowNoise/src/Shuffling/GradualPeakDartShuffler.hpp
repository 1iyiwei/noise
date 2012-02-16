/*
  GradualPeakDartShuffler.hpp

  try to move the peak in a decreasing energy order
  i.e. if i-th peak cannot be moved without increasing energy, try i+1-th

  Li-Yi Wei
  08/26/2009

*/

#ifndef _GRADUAL_PEAK_DART_SHUFFLER_HPP
#define _GRADUAL_PEAK_DART_SHUFFLER_HPP

#include "DartShuffler.hpp"

class GradualPeakDartShuffler : public DartShuffler
{
public:
    GradualPeakDartShuffler(EnergyField & energy_field, const PeakFinder & peak_finder, const TroughFinder & trough_finder, const int max_num_peaks_to_look_per_iteration);
    virtual ~GradualPeakDartShuffler(void);

    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    virtual float Next(void);

protected:
    const int _max_num_peaks_to_look_per_iteration;

    struct SortElement
    {
        SortElement(void) {sample = 0;};
        SortElement(const EnergySample & sample_in) : sample(&sample_in) {};
        ~SortElement(void) {};

        int operator<(const SortElement & rhs) const {return sample->GetValue() < rhs.sample->GetValue();};

        const EnergySample * sample;
    };

    // one list for each target class
    vector<SortElement> _list;
    SortElement _element;

    float _old_energy;
};

#endif
