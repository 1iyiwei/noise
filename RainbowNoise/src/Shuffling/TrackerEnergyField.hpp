/*
  TrackerEnergyField.hpp

  the energy field combining EnergySampleTracker and EnergyField

  Li-Yi Wei
  09/04/2009

*/

#ifndef _TRACKER_ENERGY_FIELD_HPP
#define _TRACKER_ENERGY_FIELD_HPP

#include "EnergyField.hpp"
#include "EnergySampleTracker.hpp"

#include "PlainBoxDomain.hpp"
#include "GaussianBlob.hpp"
#include "FairClassWeight.hpp"
#include "UniformFairKernelSize.hpp"

class TrackerEnergyField : public EnergyField
{
public:
    TrackerEnergyField(EnergyField & real_energy_field, EnergySampleTracker & tracker);
    virtual ~TrackerEnergyField(void);

    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    virtual int Get(vector<Sample> & samples) const;
    virtual int Get(vector<EnergySample> & samples) const;

    virtual const vector<const EnergySample *> & Get(void) const;
    virtual const vector<EnergySample *> & Get(void);

    virtual float Get(const Sample & query) const;
    virtual float Get(const EnergySample & query) const;

    virtual const EnergySample * Peak(const bool including_fixed) const;
    virtual const EnergySample * Trough(const EnergySample & peak) const;

    virtual const EnergySample * Peak(const EnergySample & query, const bool including_fixed) const;

    // internal function called by TrackerTroughFinder
    int Trough(Sample & sample) const;

    virtual float Move(EnergySample & source, const Sample & target);

    virtual float EnergyMove(const EnergySample & source, const int sign);

    virtual void Visible(EnergySample & sample, const bool value);

protected:
    int LocalSet(void);
    virtual float Get(const Sample & donor, const Sample & receiver) const;

protected:
    EnergyField & _energy_field;
    EnergySampleTracker & _tracker;

    static PlainBoxDomain _dummy_domain;
    static GaussianBlob _dummy_blob;
    static FairClassWeight _dummy_class_weight;
    static UniformFairKernelSize _dummy_kernel_size;
};
#endif
