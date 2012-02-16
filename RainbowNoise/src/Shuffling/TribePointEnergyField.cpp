/*
  TribePointEnergyField.cpp

  Li-Yi Wei
  08/11/2009

*/

#include "TribePointEnergyField.hpp"
#include "Math.hpp"
#include "Exception.hpp"

TribePointEnergyField::TribePointEnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, const vector<Sample> & points, EnergySampleStore & points_store, EnergySampleStore & samples_store) : DiscreteEnergyField(domain, blob, num_class, class_weight, kernel_size, points_store), _sample_energy_field(domain, blob, num_class, class_weight, kernel_size, samples_store)
{
    // add the initial set of points  
    {
        vector<EnergySample> containers(points.size());

        for(unsigned int i = 0; i < containers.size(); i++)
        {

#ifdef _ENERGY_MULTI_VALUE
            if(!containers[i].SetNumClass(_num_class))
            {
                throw Exception("TribePointEnergyField::TribePointEnergyField(): cannot set class num");
            }
#endif
            if(!containers[i].SetSample(points[i]))
            {
                throw Exception("TribePointEnergyField::TribePointEnergyField(): cannot set sample");
            }
        }
        
        if(! _store.Set(containers))
        {
            throw Exception("TribePointEnergyField::TribePointEnergyField(): cannot set containers");
        }
    }
}

TribePointEnergyField::~TribePointEnergyField(void)
{
    // nothing else to do
}

int TribePointEnergyField::Set(const vector<Sample> & samples)
{
    if(! _sample_energy_field.Set(samples))
    {
        return 0;
    }

    return LocalSet();
}

int TribePointEnergyField::Set(const vector<EnergySample> & samples)
{
    if(! _sample_energy_field.Set(samples))
    {
        return 0;
    }

    return LocalSet();
}

int TribePointEnergyField::LocalSet(void)
{
    const vector<EnergySample *> & containers = _store.Get();

    // initialize energy values
    for(unsigned int i = 0; i < containers.size(); i++)
    {
        if(! containers[i])
        {
            throw Exception("TribePointEnergyField::Set(): empty container");
            return 0;
        }

#ifdef _ENERGY_MULTI_VALUE
        if(! containers[i]->NumClass(_num_class))
        {
            return 0;
        }
#endif

        EnergySample & query = *containers[i];

        if(! query.GetSample(_query))
        {
            return 0;
        }

#ifdef _ENERGY_MULTI_VALUE
        for(int j = 0; j < containers[i].GetNumClass(); j++)
        {
            _query.id = j;
            if(!query.SetValue(j, Get(_query)))
            {
                return 0;
            }
        }
#else
        if(!query.SetValue(Get(query)))
        {
            return 0;
        }
#endif
    }

    return 1;
}

int TribePointEnergyField::Get(vector<Sample> & samples) const
{
    return _sample_energy_field.Get(samples);
}

int TribePointEnergyField::Get(vector<EnergySample> & samples) const
{
    return _sample_energy_field.Get(samples);
}

const vector<const EnergySample *> & TribePointEnergyField::Get(void) const
{
    return _sample_energy_field.Get();
}

const vector<EnergySample *> & TribePointEnergyField::Get(void)
{
    return _sample_energy_field.Get();
}

float TribePointEnergyField::Get(const EnergySample & query) const
{
    return _sample_energy_field.Get(query);
}

const EnergySample * TribePointEnergyField::Peak(const bool including_fixed) const
{
    return _sample_energy_field.Peak(including_fixed);
}

const EnergySample * TribePointEnergyField::Peak(const EnergySample & query, const bool including_fixed) const
{
    return _sample_energy_field.Peak(query, including_fixed);
}

const EnergySample * TribePointEnergyField::Trough(const EnergySample & peak) const
{
    if(! peak.GetSample(_sample))
    {
        throw Exception("TribePointEnergyField::Trough(): cannot get peak sample");
        return 0;
    }

    const int which_class = _sample.id;

    return Trough(which_class, 0);
}
 
const EnergySample * TribePointEnergyField::Trough(const int which_class, const bool active) const
{
    const EnergySample * trough = 0;

    if(! active) return trough;

    const vector<EnergySample *> & containers = _store.Get();

    {
        float min_value = Math::INF;
        for(unsigned int i = 0; i < containers.size(); i++)
        {
            const EnergySample * current = containers[i];

            if(!current)
            {
                throw Exception("TribePointEnergyField::Trough(): null container");
                return 0;
            }

#ifdef _ENERGY_MULTI_VALUE
            {            
                float value = 0.0;
                if(! current->GetValue(which_class, value)) return 0;

                if(value < min_value)
                {
                    min_value = value;
                    trough = current;
                }
            }
#else
            if(! current->GetSample(_sample)) return 0;

            if(_sample.id == which_class)
            {
                float value = 0.0;
                if(! current->GetValue(value)) return 0;
            
                if(value < min_value)
                {
                    min_value = value;
                    trough = current;
                }
            }
#endif
        }
    }

    return trough;
}

float TribePointEnergyField::Move(EnergySample & source, const Sample & target)
{
    if(!_target.SetSample(target))
    {
        throw Exception("TribePointEnergyField::Move(): cannot set target sample");
        return Math::NAN;
    }

    DiscreteEnergyField::EnergyMove(source, -1);
    // DiscreteEnergyField::EnergyMove(_target, +1);

    const float value = _sample_energy_field.Move(source, target);

    DiscreteEnergyField::EnergyMove(source, +1);

    return value;
}

float TribePointEnergyField::EnergyMove(const EnergySample & source, const int sign)
{
    if(Math::IsNAN(DiscreteEnergyField::EnergyMove(source, sign)))
    {
        return Math::NAN;
    }

    return _sample_energy_field.EnergyMove(source, sign);
}
