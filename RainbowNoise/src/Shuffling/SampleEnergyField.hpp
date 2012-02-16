/*
  SampleEnergyField.hpp

  energy field with values computed and stored on the samples

  Li-Yi Wei
  06/15/2009

*/

#ifndef _SAMPLE_ENERGY_FIELD_HPP
#define _SAMPLE_ENERGY_FIELD_HPP

#include "DiscreteEnergyField.hpp"
#include "EnergySampleStore.hpp"

class SampleEnergyField : public DiscreteEnergyField
{
public:
    SampleEnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, EnergySampleStore & store);

    virtual ~SampleEnergyField(void);

    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    virtual int Get(vector<Sample> & samples) const;
    virtual int Get(vector<EnergySample> & samples) const;

    virtual const vector<const EnergySample *> & Get(void) const;
    virtual const vector<EnergySample *> & Get(void);

    virtual float Get(const EnergySample & query) const;

    virtual const EnergySample * Peak(const bool including_fixed) const;
    virtual const EnergySample * Peak(const EnergySample & query, const bool including_fixed) const;

protected:
    virtual const EnergySample * Peak(const vector<const EnergySample *> & candidates, const bool including_fixed) const;

protected:
    mutable Sample _center, _query, _sample;
};
#endif
