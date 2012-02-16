/*
  EnergyField.hpp

  base class for all energy fields, answering queries about energy

  Li-Yi Wei
  06/10/2009

*/

#ifndef _ENERGY_FIELD_HPP
#define _ENERGY_FIELD_HPP

#include <memory>
using namespace std;

#include "Domain.hpp"
#include "EnergySample.hpp"
#include "SingleBlob.hpp"
#include "UnionBlob.hpp"

class EnergyField
{
public:
    EnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size);
    virtual ~EnergyField(void) = 0;

    // initialization
    virtual int Set(const vector<Sample> & samples);
    virtual int Set(const vector<EnergySample> & samples);

    // get the sample set
    virtual int Get(vector<Sample> & samples) const;
    virtual int Get(vector<EnergySample> & samples) const;

    // get the energy sample set
    virtual const vector<const EnergySample *> & Get(void) const;
    virtual const vector<EnergySample *> & Get(void);
    
    // query energy value
    virtual float Get(const Sample & query) const;
    virtual float Get(const EnergySample & query) const;

    // find peak/trough
    // return 0 if failure
    virtual const EnergySample * Peak(const bool including_fixed) const;
    virtual const EnergySample * Trough(const EnergySample & peak) const;

    // peak in the neighborhood of query
    virtual const EnergySample * Peak(const EnergySample & query, const bool including_fixed) const;

    // return the diff energy caused by plus/minus of sample, NAN if error
    float EnergyPlus(const EnergySample & source);
    float EnergyMinus(const EnergySample & source);

    // assuming source is already part of energy sample field
    // (in terms of & source)
    // return the effective energy difference if successful, nan else
    virtual float Move(EnergySample & source, const Sample & target);

    virtual void Visible(EnergySample & sample, const bool value);

    // get domain
    const Domain & GetDomain(void) const;

    int NumClass(void) const;

    // update energy sample field, and
    // return the diff energy caused by plus/minus of sample
    virtual float EnergyMove(const EnergySample & source, const int sign);

protected:
    friend class TrackerEnergyField;

    // return the energy impact of donor upon receiver
    virtual float Get(const Sample & donor, const Sample & receiver) const;

protected:
    const Domain & _domain;
    const int _num_class;
    mutable UnionBlob _blob;

    const vector<EnergySample *> _empty;
    const vector<const EnergySample *> _const_empty;

    mutable EnergySample _query;
};
#endif
