/*
  UnionPointEnergyField.hpp

  computing energy values on a set of discrete points 
  in the jargon of [Balzer et al. SIGGRAPH 2009]

  the points are shared across all classes

  Li-Yi Wei
  08/21/2009

*/

#ifndef _UNION_POINT_ENERGY_FIELD_HPP
#define _UNION_POINT_ENERGY_FIELD_HPP

#include "DiscreteEnergyField.hpp"
#include "EnergySampleStore.hpp"

class UnionPointEnergyField : public DiscreteEnergyField
{
public:
    UnionPointEnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, EnergySampleStore & store);

    virtual ~UnionPointEnergyField(void);

    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    virtual int Get(vector<Sample> & samples) const;
    virtual int Get(vector<EnergySample> & samples) const;

    virtual const vector<const EnergySample *> & Get(void) const;
    virtual const vector<EnergySample *> & Get(void);

    virtual float Get(const EnergySample & query) const;
    virtual float Get(const EnergySample & query, const int which_class) const;

    virtual const EnergySample * Peak(const bool including_fixed) const;
    virtual const EnergySample * Peak(const EnergySample & query, const bool including_fixed) const;
    virtual const EnergySample * Trough(const EnergySample & peak) const;

protected:
    virtual const EnergySample * Peak(const vector<const EnergySample *> & candidates, const bool including_fixed) const;

    // finding trough with min energy if becoming target_class
    virtual const EnergySample * Trough(const int target_class) const;
    static float GetValue(const EnergySample & sample, const int which_class);
public:

    virtual float EnergyMove(const EnergySample & source, const int sign);

protected:
    mutable Sample _center, _query, _sample;
};
#endif
