/*
  EnergySampleField.cpp

  Li-Yi Wei
  08/17/2009

*/

#include "EnergySampleField.hpp"
#include "Math.hpp"
#include "Exception.hpp"

EnergySampleField::EnergySampleField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, EnergySampleStore & store) : _domain(domain), _blob(blob, num_class, class_weight, kernel_size), _num_class(num_class), _store(store)
{
    // nothing else to do
}

EnergySampleField::~EnergySampleField(void)
{
    // nothing else to do
}

int EnergySampleField::Set(const vector<Sample> & samples)
{
    {
        vector<EnergySample> containers(samples.size());

        for(unsigned int i = 0; i < containers.size(); i++)
        {

#ifdef _ENERGY_MULTI_VALUE
            if(!containers[i].SetNumClass(_num_class))
            {
                return 0;
            }
#endif
            if(!containers[i].SetSample(samples[i]))
            {
                return 0;
            }
        }
        
        if(! _store.Set(containers))
        {
            return 0;
        }
    }

    const vector<EnergySample *> & containers = _store.Get();

    Sample query;

    // initialize energy values
    for(unsigned int i = 0; i < containers.size(); i++)
    {
        if(! containers[i]->GetSample(query))
        {
            return 0;
        }

#ifdef _ENERGY_MULTI_VALUE
        const int original_id = query.id;
        for(int j = 0; j < containers[i].GetNumClass(); j++)
        {
            query.id = j;
            if(! containers[i]->SetSample(query)) 
            {
                return 0;
            }

            if(!containers[i]->SetValue(j, Get(*containers[i])))
            {
                return 0;
            }
        }
        
        query.id = original_id;
        if(! containers[i]->SetSample(query)) 
        {
            return 0;
        }
#else
        if(!containers[i]->SetValue(Get(*containers[i])))
        {
            return 0;
        }
#endif
    }

    return 1;
}

int EnergySampleField::Get(vector<Sample> & samples) const
{
    vector<EnergySample> containers;
    if(! _store.Get(containers))
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

#if 0
const vector<const EnergySample *> & EnergySampleField::Get(void) const
{
    return _store.Get();
}
#endif

const vector<EnergySample *> & EnergySampleField::Get(void)
{
    return _store.Get();
}

const Domain & EnergySampleField::GetDomain(void) const
{
    return _domain;
}

float EnergySampleField::Get(const EnergySample & query) const
{
    if(! query.GetSample(_query))
    {
        throw Exception("EnergySampleField::Get(): cannot get sample");
        return Math::NAN;
    }

    float value = 0;

    // const vector<const EnergySample *> & containers = _store.Get(query);
    const vector<EnergySample *> & containers = _store.Get(_query);

    for(unsigned int i = 0; i < containers.size(); i++)
    { 
        if(!containers[i])
        {
            throw Exception("EnergySampleField::Get(): null container");
            return Math::NAN;
        }

        if(containers[i] == &query) continue; // don't double count self

        if(! containers[i]->GetSample(_center))
        {
            throw Exception("EnergySampleField::Get(): cannot get center");
            return Math::NAN;
        }

        if(_center.id < 0) continue; // don't count in points

        if(!_blob.SetCenter(_center))
        {
            throw Exception("EnergySampleField::Get(): cannot set center");
            return Math::NAN;
        }

        if(!_domain.Nearest(_center, _query, _query_nearest))
        {
            throw Exception("EnergySampleField::Get(): cannot get nearest");
            return Math::NAN;
        }

        value += _blob.Get(_query_nearest);
    }

    return value;
}

float EnergySampleField::Move(EnergySample & source, const Sample & target)
{
    if(! source.GetSample(_sample))
    {
        throw Exception("EnergySampleField::Move(): cannot get sample");
        return Math::NAN;
    }

    if(_sample.id != target.id)
    {
        throw Exception("EnergySampleField::Move(): class mismatch");
        return Math::NAN;
    }

    const float diff_neighbor_minus = EnergyMove(source, -1);
    const float my_source = Get(source);

    if(! _store.Move(source, target))
    {
        throw Exception("EnergySampleField::Move(): cannot move sample");
        return Math::NAN;
    }

    const float diff_neighbor_plus = EnergyMove(source, +1);
    const float my_target = Get(source);
    if(! source.SetValue(my_target))
    {
        throw Exception("EnergySampleField::Move(): cannot set energy");
        return Math::NAN;
    }

    return (diff_neighbor_minus + diff_neighbor_plus + my_target - my_source);
}

float EnergySampleField::EnergyMove(const EnergySample & source, const int sign)
{
    if(! source.GetSample(_sample))
    {
        throw Exception("EnergySampleField::EnergyMove(): cannot get sample");
        return Math::NAN;
    }

    if(_sample.id < 0)
    {
        return 0;
    }

    const vector<EnergySample *> & containers = _store.Get(_sample);

    // update energy of existing samples
    if(!_blob.SetCenter(_sample))
    {
        throw Exception("EnergySampleField::EnergyMove(): cannot set center");
        return Math::NAN;
    }

    float neighbor_diff = 0;
    float value = 0;
    for(unsigned int i = 0; i < containers.size(); i++)
    {
        if(! containers[i])
        {
            throw Exception("EnergySampleField::EnergyMove(): null container");
            return Math::NAN;
        }

        if(containers[i] == &source) continue; // don't double count self

        if(! containers[i]->GetSample(_query))
        {
            throw Exception("EnergySampleField::EnergyMove(): cannot get sample");
            return Math::NAN;
        }
                
        if(_query.id < 0) continue; // point

#ifdef _ENERGY_MULTI_VALUE
        for(int j = 0; j < containers[i]->GetNumClass(); j++)
        {
            if(! containers[i]->GetValue(j, value))
            {
                throw Exception("EnergySampleField::EnergyMove(): cannot get value");
                return Math::NAN;
            }
        
            _query.id = j;
            const float diff = sign*_blob.Get(_query);
            neighbor_diff += diff;
            value += diff;
        
            if(! containers[i]->SetValue(j, value))
            {
                throw Exception("EnergySampleField::EnergyMove(): cannot set value");
                return Math::NAN;
            }
        }
#else
        if(! containers[i]->GetValue(value))
        {
            throw Exception("EnergySampleField::EnergyMove(): cannot get value");
            return Math::NAN;
        }
        
        const float diff = sign*_blob.Get(_query);
        neighbor_diff += diff;
        value += diff;
        
        if(! containers[i]->SetValue(value))
        {
            throw Exception("EnergySampleField::EnergyMove(): cannot set value");
            return Math::NAN;
        }
#endif
    }

    // done
    return neighbor_diff;
}
