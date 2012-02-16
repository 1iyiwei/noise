/*
  UnionPointEnergyField.cpp

  Li-Yi Wei
  08/21/2009

*/

#include <algorithm>
using namespace std;

#include <math.h>

#include "UnionPointEnergyField.hpp"
#include "Math.hpp"
#include "Exception.hpp"

UnionPointEnergyField::UnionPointEnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, EnergySampleStore & store) : DiscreteEnergyField(domain, blob, num_class, class_weight, kernel_size, store)
{
    // nothing else to do
}

UnionPointEnergyField::~UnionPointEnergyField(void)
{
    // nothing else to do
}

int UnionPointEnergyField::Set(const vector<Sample> & samples)
{ 
    {
        vector<EnergySample> containers(samples.size());

        for(unsigned int i = 0; i < containers.size(); i++)
        {
            if(!containers[i].NumClass(_num_class))
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
 
int UnionPointEnergyField::Set(const vector<EnergySample> & input_containers)
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

        if(!containers[i]->NumClass(_num_class))
        {
            return 0;
        }

        EnergySample & query = *containers[i];

        if(! query.GetSample(_query))
        {
            return 0;
        }

        {
            for(int j = 0; j < query.NumClass(); j++)
            {
                if(!query.SetValue(j, Get(query, j)))
                {
                    return 0;
                }
            }
        }
        
        if(_query.id >= 0)
        {
            float value = 0;
            if(! query.GetValue(_query.id, value))
            {
                value = Get(query);
            }

            if(!query.SetValue(value))
            {
                return 0;
            }
        }
    }

    return 1;
}

int UnionPointEnergyField::Get(vector<Sample> & samples) const
{
    return DiscreteEnergyField::Get(samples);
}

int UnionPointEnergyField::Get(vector<EnergySample> & samples) const
{
    return DiscreteEnergyField::Get(samples);
}

const vector<const EnergySample *> & UnionPointEnergyField::Get(void) const
{
    return DiscreteEnergyField::Get();
}

const vector<EnergySample *> & UnionPointEnergyField::Get(void)
{
    return DiscreteEnergyField::Get();
}

float UnionPointEnergyField::Get(const EnergySample & query) const
{
    return DiscreteEnergyField::Get(query);
}

float UnionPointEnergyField::Get(const EnergySample & query, const int which_class) const
{
    return DiscreteEnergyField::Get(query, which_class);
}

const EnergySample * UnionPointEnergyField::Peak(const bool including_fixed) const
{
    const vector<const EnergySample *> & samples = const_cast<const EnergySampleStore &>(_store).Get();

    return Peak(samples, including_fixed);
}

const EnergySample * UnionPointEnergyField::Peak(const EnergySample & query, const bool including_fixed) const
{
    return DiscreteEnergyField::Peak(query, including_fixed);
}

const EnergySample * UnionPointEnergyField::Peak(const vector<const EnergySample *> & samples, const bool including_fixed) const
{
    float max_energy = -Math::INF;
    float energy = 0;
    const EnergySample * peak = 0;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!samples[i] || (!samples[i]->GetSample(_sample)) || (!samples[i]->GetValue(energy)))
        {
            throw Exception("UnionPointEnergyField::Peak(): cannot get sample information");
            return 0;
        }

        if(!including_fixed && samples[i]->Fixed()) continue;

        if((_sample.id >= 0) && (energy > max_energy))
        {
            max_energy = energy;
            peak = samples[i];
        }
    }

    return peak;
}

const EnergySample * UnionPointEnergyField::Trough(const EnergySample & peak) const
{
    if(! peak.GetSample(_sample))
    {
        throw Exception("UnionPointEnergyField::Trough(): cannot get peak sample");
        return 0;
    }

#if 1
    const int source_class = _sample.id;

    float min_energy = Math::INF;
    const EnergySample * trough = 0;

    for(int target_class = -1; target_class < NumClass(); target_class++)
    {
        if(target_class == source_class) continue;

        const float source_energy = GetValue(peak, target_class);

        const EnergySample * target_trough = Trough(source_class);

        if(target_trough)
        {
            const float target_energy = GetValue(*target_trough, source_class);

            const float energy = max(source_energy, target_energy);

            if(energy < min_energy)
            {
                min_energy = energy;
                trough = target_trough;
            }
        }
    }

#else
    const int which_class = _sample.id;

    const vector<const EnergySample *> & samples = const_cast<const EnergySampleStore &>(_store).Get();

    float min_energy = Math::INF;
    float energy = Math::INF;
    const EnergySample * trough = 0;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!samples[i] || (!samples[i]->GetSample(_sample)))
        {
            throw Exception("UnionPointEnergyField::Trough(): cannot get sample information");
            return 0;
        }

        if(_sample.id == which_class) continue; // only swap with a sample in different class

        if(samples[i]->NumClass() > 0) // point/sample with multi-class energy
        // if(_sample.id < 0) // point
        {
            if(! samples[i]->GetValue(which_class, energy))
            {
                throw Exception("UnionPointEnergyField::Trough(): cannot get energy value");
                return 0;
            }
        }
        else if(_sample.id != which_class) // sample with the right class id
        {
            if(! samples[i]->GetValue(energy))
            {
                throw Exception("UnionPointEnergyField::Trough(): cannot get energy value");
                return 0;
            }
        }
        else
        {
            energy = Math::INF;
        }

        if(energy < min_energy)
        {
            min_energy = energy;
            trough = samples[i];
        }
    }
#endif

    return trough;
}

const EnergySample * UnionPointEnergyField::Trough(const int target_class) const
{
    const vector<const EnergySample *> & samples = const_cast<const EnergySampleStore &>(_store).Get();

    float min_energy = Math::INF;
    const EnergySample * trough = 0;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!samples[i] || (!samples[i]->GetSample(_sample)))
        {
            throw Exception("UnionPointEnergyField::Trough(): cannot get sample information");
            return 0;
        }

        if(_sample.id == target_class) continue; // already belongs to target class, so skip

        const float energy = GetValue(*samples[i], target_class);

        if(energy < min_energy)
        {
            min_energy = energy;
            trough = samples[i];
        }
    }

    return trough;
}

float UnionPointEnergyField::GetValue(const EnergySample & sample, const int which_class)
{   
    float energy = 0;

    if(which_class < 0)
    {
        energy = 0;
    }
    else
    {
        if(! sample.GetValue(which_class, energy))
        {
            throw Exception("UnionPointEnergyField::GetValue(): cannot get energy");
            return Math::NAN;
        }
    }

    return energy;
}

float UnionPointEnergyField::EnergyMove(const EnergySample & source, const int sign)
{
    return DiscreteEnergyField::EnergyMove(source, sign);
}
