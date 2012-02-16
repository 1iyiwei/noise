/*
  EnergySampleTracker.hpp

  the base class for all energy sample trackers
  
  tracking the distribution of the samples and
  return potential peak/trough candidates 
  acting like an oracle

  Li-Yi Wei
  09/03/2009

*/

#ifndef _ENERGY_SAMPLE_TRACKER_HPP
#define _ENERGY_SAMPLE_TRACKER_HPP

#include "EnergySample.hpp"

class EnergySampleTracker
{
public:
    virtual ~EnergySampleTracker(void) = 0;

    virtual int Set(const vector<const EnergySample *> & samples);

    int Plus(const EnergySample & sample);
    int Minus(const EnergySample & sample);

    // return peak/trough among existing energy samples
    virtual const EnergySample * Peak(void) const;
    virtual const EnergySample * Trough(const EnergySample & peak) const;

    // return trough location outside existing energy samples
    // note that the Trough here is only a candidate
    // and the caller needs to call these functions repeatedly
    // to pick the best one
    virtual int Trough(Sample & sample) const;

protected:
    friend class UnionEnergySampleTracker;

    typedef enum {PLUS, MINUS} PlusMinusOption;
    virtual int PlusMinus(const EnergySample & sample, const PlusMinusOption plus_or_minus);
};

#endif

