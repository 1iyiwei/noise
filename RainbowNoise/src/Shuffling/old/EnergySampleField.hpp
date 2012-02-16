/*
  EnergySampleField.hpp

  energy field that stores energy samples
  and allows only moving samples, not addition/deletion as in EnergyField

  Li-Yi Wei
  08/17/2009

*/

#ifndef _ENERGY_SAMPLE_FIELD_HPP
#define _ENERGY_SAMPLE_FIELD_HPP

#include "Domain.hpp"
#include "Sample.hpp"
#include "SingleBlob.hpp"
#include "UnionBlob.hpp"
#include "ClassWeight.hpp"
#include "KernelSize.hpp"
#include "EnergySampleStore.hpp"

class EnergySampleField
{
public:
    EnergySampleField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, EnergySampleStore & store);

    virtual ~EnergySampleField(void);

    // initialization
    virtual int Set(const vector<Sample> & samples);

    // get the sample set
    virtual int Get(vector<Sample> & samples) const;

    // get the energy sample set
    // virtual const vector<const EnergySample *> & Get(void) const;
    virtual const vector<EnergySample *> & Get(void);

    // get domain
    const Domain & GetDomain(void) const;

    // get energy value at a particular query sample location
    // caution: class info query.id plays a role here!
    virtual float Get(const EnergySample & query) const;

    // assuming source is already part of energy sample field
    // (in terms of & source)
    // return the effective energy difference if successful, nan else
    virtual float Move(EnergySample & source, const Sample & target);

protected:
    // update energy sample field, and
    // return the diff energy caused by plus/minus of sample
    float EnergyMove(const EnergySample & source, const int sign);

protected:
    const Domain & _domain;
    mutable UnionBlob _blob;

    const int _num_class;

    EnergySampleStore & _store;

    // use these variables very very carefully
    mutable Sample _center, _query, _query_nearest, _sample;
};
#endif

  
