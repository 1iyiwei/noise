/*
  DiscreteEnergyField.hpp

  energy field with values computed and stored on a discrete set

  Li-Yi Wei
  08/10/2009

*/

#ifndef _DISCRETE_ENERGY_FIELD_HPP
#define _DISCRETE_ENERGY_FIELD_HPP

#include "EnergyField.hpp"
#include "EnergySampleStore.hpp"

class DiscreteEnergyField : public EnergyField
{
public:
    DiscreteEnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, EnergySampleStore & store);

    virtual ~DiscreteEnergyField(void) = 0;

    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    virtual int Get(vector<Sample> & samples) const;
    virtual int Get(vector<EnergySample> & samples) const;

    virtual const vector<const EnergySample *> & Get(void) const;
    virtual const vector<EnergySample *> & Get(void);

    virtual float Get(const Sample & query) const;
    virtual float Get(const EnergySample & query) const;
    virtual float Get(const EnergySample & query, const int which_class) const;

    virtual const EnergySample * Peak(const EnergySample & query, const bool including_fixed) const;

    virtual float Move(EnergySample & source, const Sample & target);

    virtual float EnergyMove(const EnergySample & source, const int sign);

protected:
    virtual const EnergySample * Peak(const vector<const EnergySample *> & candidates, const bool including_fixed) const;
    virtual float Get(const Sample & donor, const Sample & receiver) const;

protected:

    EnergySampleStore & _store;

    // use these variables very carefully to avoid conflicts
    mutable Sample _center, _query, _query_id, _query_nearest, _sample;
};
#endif
