/*
  TribePointEnergyField.hpp

  computing energy values on a set of discrete points 
  in the jargon of [Balzer et al. SIGGRAPH 2009]

  each point belong to a specific class and is not shared

  Li-Yi Wei
  08/11/2009

*/

#ifndef _TRIBE_POINT_ENERGY_FIELD_HPP
#define _TRIBE_POINT_ENERGY_FIELD_HPP

#include "DiscreteEnergyField.hpp"
#include "SampleEnergyField.hpp"
#include "EnergySampleStore.hpp"

class TribePointEnergyField : public DiscreteEnergyField
{
public:
    TribePointEnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, const vector<Sample> & points, EnergySampleStore & points_store, EnergySampleStore & samples_store);

    virtual ~TribePointEnergyField(void);

    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    virtual int Get(vector<Sample> & samples) const;
    virtual int Get(vector<EnergySample> & samples) const;

    virtual const vector<const EnergySample *> & Get(void) const;
    virtual const vector<EnergySample *> & Get(void);

    virtual float Get(const EnergySample & query) const;

    virtual const EnergySample * Peak(const bool including_fixed) const;
    virtual const EnergySample * Peak(const EnergySample & query, const bool including_fixed) const;
    
    virtual const EnergySample * Trough(const EnergySample & peak) const;
    // internal function called by DiscreteTroughFinder
    const EnergySample * Trough(const int which_class, const bool active) const;

    virtual float Move(EnergySample & source, const Sample & target);

    virtual float EnergyMove(const EnergySample & source, const int sign);

protected:
    int LocalSet(void);

protected:
    SampleEnergyField _sample_energy_field;
    mutable Sample _center, _query, _sample;
    mutable EnergySample _target;
};
#endif
