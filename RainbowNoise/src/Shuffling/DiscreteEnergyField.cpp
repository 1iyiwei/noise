/*
  DiscreteEnergyField.cpp

  Li-Yi Wei
  08/10/2009

*/

#include <math.h>

#include "DiscreteEnergyField.hpp"
#include "ShuffleTools.hpp"
#include "Math.hpp"
#include "Exception.hpp"

// #define _INVISIBLE_TRANS_CLASS // not working 08/22/2009
#define _GET_DONOR_RECEIVER // use shared code for Get(donor, receiver)

DiscreteEnergyField::DiscreteEnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, EnergySampleStore & store) : EnergyField(domain, blob, num_class, class_weight, kernel_size), _store(store)
{
    // nothing else to do
}

DiscreteEnergyField::~DiscreteEnergyField(void)
{
    // nothing else to do
}

int DiscreteEnergyField::Set(const vector<Sample> & samples)
{
    {
        vector<EnergySample> containers(samples.size());

        for(unsigned int i = 0; i < containers.size(); i++)
        {
            if((samples[i].id < 0) && !containers[i].NumClass(_num_class))
            {
                return 0;
            }

            if(!containers[i].SetSample(samples[i]))
            {
                return 0;
            }
        }

        return Set(containers);
    }
}

int DiscreteEnergyField::Set(const vector<EnergySample> & input_containers)
{
    if(! _store.Set(input_containers))
    {
        return 0;
    }

    const vector<EnergySample *> & containers = _store.Get();

    // initialize energy values
    for(unsigned int i = 0; i < containers.size(); i++)
    {
        if(! containers[i])
        {
            throw Exception("DiscreteEnergyField::Set(): null container");
            return 0;
        }

        EnergySample & query = *containers[i];

        if(! query.GetSample(_query))
        {
            return 0;
        }

        if((_query.id < 0) && !containers[i]->NumClass(_num_class))
        {
            throw Exception("DiscreteEnergyField::Set(): cannot set num class");
            return 0;
        }

        if(_query.id < 0)
        {
            for(int j = 0; j < query.NumClass(); j++)
            {
                if(!query.SetValue(j, Get(query, j)))
                {
                    return 0;
                }
            }
        }
        else
        {
            if(!query.SetValue(Get(query)))
            {
                return 0;
            }
        }
    }

    return 1;
}

int DiscreteEnergyField::Get(vector<Sample> & samples) const
{
    vector<EnergySample> containers;
    if(! Get(containers))
    {
        return 0;
    }

    samples = vector<Sample>(containers.size());

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!containers[i].GetSample(samples[i]))
        {
            return 0;
        }
    }

    return 1;
}

int DiscreteEnergyField::Get(vector<EnergySample> & containers) const
{
    return _store.Get(containers);
}

const vector<const EnergySample *> & DiscreteEnergyField::Get(void) const
{
    return const_cast<const EnergySampleStore &>(_store).Get();
}

const vector<EnergySample *> & DiscreteEnergyField::Get(void)
{
    return _store.Get();
}

float DiscreteEnergyField::Get(const Sample & query) const
{
    return EnergyField::Get(query);
}

float DiscreteEnergyField::Get(const EnergySample & query) const
{ 
    if(! query.GetSample(_query))
    {
        throw Exception("DiscreteEnergyField::Get(): cannot get sample");
        return Math::NAN;
    }

    return Get(query, _query.id);
}

float DiscreteEnergyField::Get(const EnergySample & query, const int which_class) const
{ 
    if(! query.GetSample(_query_id))
    {
        throw Exception("DiscreteEnergyField::Get(): cannot get sample");
        return Math::NAN;
    }

    const bool trans_class = (_query_id.id != which_class);

    _query_id.id = which_class;

    float value = 0;

    // const vector<const EnergySample *> & containers = _store.Get(query);
    const vector<EnergySample *> & containers = _store.Get(_query_id);

    for(unsigned int i = 0; i < containers.size(); i++)
    { 
        if(!containers[i])
        {
            throw Exception("DiscreteEnergyField::Get(): null container");
            return Math::NAN;
        }

        if(!containers[i]->Visible()) continue; // don't count invisible samples

#ifdef _INVISIBLE_TRANS_CLASS
        if((containers[i] == &query) && trans_class) continue; // don't count myself for trans-class case
#endif

        if(! containers[i]->GetSample(_center))
        {
            throw Exception("DiscreteEnergyField::Get(): cannot get center");
            return Math::NAN;
        }

        if(_center.id < 0) continue; // don't count in points

#ifdef _GET_DONOR_RECEIVER
        value += Get(_center, _query_id);
#else
        if(!_blob.SetCenter(_center))
        {
            throw Exception("DiscreteEnergyField::Get(): cannot set center");
            return Math::NAN;
        }

        if(!_domain.Nearest(_center, _query_id, _query_nearest))
        {
            throw Exception("DiscreteEnergyField::Get(): cannot get nearest");
            return Math::NAN;
        }

        value += _blob.Get(_query_nearest);
#endif
    }

    return value;
}

const EnergySample * DiscreteEnergyField::Peak(const EnergySample & query, const bool including_fixed) const
{
    if(! query.GetSample(_query))
    {
        throw Exception("DiscreteEnergyField::Move(): cannot get sample");
        return 0;
    }

    const vector<const EnergySample *> & candidates = const_cast<const EnergySampleStore &>(_store).Get(_query);

    return Peak(candidates, including_fixed);
}

float DiscreteEnergyField::Move(EnergySample & source, const Sample & target)
{
    if(! source.GetSample(_sample))
    {
        throw Exception("DiscreteEnergyField::Move(): cannot get sample");
        return Math::NAN;
    }

    if(_sample.id != target.id)
    {
        throw Exception("DiscreteEnergyField::Move(): class mismatch");
        return Math::NAN;
    }

    // const float my_source = Get(source);
    const float diff_neighbor_minus = EnergyMove(source, -1);

    if(! _store.Move(source, target))
    {
        throw Exception("DiscreteEnergyField::Move(): cannot move sample");
        return Math::NAN;
    } 

    const float diff_neighbor_plus = EnergyMove(source, +1);

    if(! source.GetSample(_sample))
    {
        throw Exception("DiscreteEnergyField::Move(): cannot get sample");
        return Math::NAN;
    }

    if(_sample.id < 0)
    {
        if(source.NumClass() != NumClass())
        {
            source.NumClass(NumClass());
        }
    }

    // apply to both point or sample with multi-class information
    // this is necessary to identify non-point targets 
    {
        for(int j = 0; j < source.NumClass(); j++)
        {
            const float my_target = Get(source, j);
            if(! source.SetValue(j, my_target))
            {
                throw Exception("DiscreteEnergyField::Move(): cannot set energy");
                return Math::NAN;
            }
        }
    }

    if(_sample.id >= 0) // sample
    {
        float my_target = 0;

        if(!source.GetValue(_sample.id, my_target))
        {
            my_target = Get(source);
        }

        if(! source.SetValue(my_target))
        {
            throw Exception("DiscreteEnergyField::Move(): cannot set energy");
            return Math::NAN;
        }
    }

    return (diff_neighbor_minus + diff_neighbor_plus); //  + my_target - my_source);
}

float DiscreteEnergyField::EnergyMove(const EnergySample & source, const int sign)
{
    if(! source.GetSample(_sample))
    {
        throw Exception("DiscreteEnergyField::EnergyMove(): cannot get sample");
        return Math::NAN;
    }

    if(_sample.id < 0)
    {
        return 0;
    }

    const vector<EnergySample *> & containers = _store.Get(_sample);

#ifndef _GET_DONOR_RECEIVER
    // update energy of existing samples
    if(!_blob.SetCenter(_sample))
    {
        throw Exception("DiscreteEnergyField::EnergyMove(): cannot set center");
        return Math::NAN;
    }
#endif

    float neighbor_diff = 0;
    float value = 0;
    for(unsigned int i = 0; i < containers.size(); i++)
    {
        if(! containers[i])
        {
            throw Exception("DiscreteEnergyField::EnergyMove(): null container");
            return Math::NAN;
        }

        EnergySample & query = *containers[i];

        if(! query.GetSample(_query))
        {
            throw Exception("DiscreteEnergyField::EnergyMove(): cannot get sample");
            return Math::NAN;
        }
                
        const int query_id = _query.id;

#ifndef _GET_DONOR_RECEIVER
        if(!_domain.Nearest(_sample, _query, _query_nearest))
        {
            throw Exception("DiscreteEnergyField::EnergyMove(): cannot get nearest");
            return Math::NAN;
        }
#endif
        // point/sample with multi-class information
        if(query.NumClass() > 0)
        {
            for(int j = 0; j < query.NumClass(); j++)
            {
#ifdef _INVISIBLE_TRANS_CLASS
                if((containers[i] == &source) && (j != query_id)) continue; // don't count myself for trans-class case
#endif
                if(! query.GetValue(j, value))
                {
                    throw Exception("DiscreteEnergyField::EnergyMove(): cannot get value");
                    return Math::NAN;
                }

#ifdef _GET_DONOR_RECEIVER
                _query.id = j;
                const float diff = sign*Get(_sample, _query);
#else
                _query_nearest.id = j;
                const float diff = sign*_blob.Get(_query_nearest);
#endif
                // neighbor_diff += diff; // ignore this for points
                value += diff;
        
                if(! query.SetValue(j, value))
                {
                    throw Exception("DiscreteEnergyField::EnergyMove(): cannot set value");
                    return Math::NAN;
                }

                if(query_id == j) // sample
                {
                    neighbor_diff += diff;
                    if(! query.SetValue(value))
                    {
                        throw Exception("DiscreteEnergyField::EnergyMove(): cannot set value");
                        return Math::NAN;
                    }
                }
            }
        }
        else if(query_id >= 0) // sample without multi-class information
        {
            if(! query.GetValue(value))
            {
                throw Exception("DiscreteEnergyField::EnergyMove(): cannot get value");
                return Math::NAN;
            }

#ifdef _GET_DONOR_RECEIVER
            _query.id = query_id;
            const float diff = sign*Get(_sample, _query);
#else
            _query_nearest.id = query_id;
            const float diff = sign*_blob.Get(_query_nearest);
#endif
            neighbor_diff += diff;
            value += diff;
        
            if(! query.SetValue(value))
            {
                throw Exception("DiscreteEnergyField::EnergyMove(): cannot set value");
                return Math::NAN;
            }
        }
    }

    // done
    return neighbor_diff;
}

const EnergySample * DiscreteEnergyField::Peak(const vector<const EnergySample *> & candidates, const bool including_fixed) const
{
    throw Exception("DiscreteEnergyField::Peak(): shouldn't be called");
    return 0;
}

float DiscreteEnergyField::Get(const Sample & donor, const Sample & receiver) const
{
    if(donor.id < 0)
    {
        return 0;
    }

    if(!_blob.SetCenter(donor))
    {
        throw Exception("DiscreteEnergyField::Get(): cannot set center");
        return Math::NAN;
    } 

    if(!_domain.Nearest(donor, receiver, _query_nearest))
    {
        throw Exception("DiscreteEnergyField::EnergyMove(): cannot get nearest");
        return Math::NAN;
    }

    return _blob.Get(_query_nearest);
}
