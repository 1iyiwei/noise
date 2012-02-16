/*
  EnergySampleStore.hpp

  the base class for all energy sample store, e.g. bag, grid, tree
  should be used only for SampleEnergyField class
  
  Li-Yi Wei
  07/07/2009

*/

#ifndef _ENERGY_SAMPLE_STORE_HPP
#define _ENERGY_SAMPLE_STORE_HPP

#include "EnergySample.hpp"

class EnergySampleStore
{
public:
    virtual ~EnergySampleStore(void) = 0;

    virtual EnergySampleStore * Clone(void) const;

    virtual int Set(const vector<EnergySample> & samples);
    virtual int Get(vector<EnergySample> & samples) const;

    // get all samples
    virtual const vector<const EnergySample *> & Get(void) const;
    virtual const vector<EnergySample *> & Get(void);

    // get neighbors
    virtual const vector<const EnergySample *> & Get(const Sample & query) const;
    virtual const vector<EnergySample *> & Get(const Sample & query);

    virtual int Plus(const EnergySample & rookie);
    // index is assumed to be consistent with the vector of all samples
    virtual int Minus(const int index);

    // move the location of a energy sample
    // assuming source is already part of this energy store
    virtual int Move(EnergySample & source, const Sample & target);

protected:
    vector<EnergySample> _samples;
    vector<const EnergySample *> _const_p_samples;
    vector<EnergySample *> _p_samples;

    mutable Sample _sample;
};

#endif
