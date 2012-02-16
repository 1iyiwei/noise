/*
  SampleEnergyField.cpp

  Li-Yi Wei
  06/15/2009

*/

#include <math.h>

#include "SampleEnergyField.hpp"
#include "Math.hpp"
#include "Exception.hpp"

SampleEnergyField::SampleEnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size, EnergySampleStore & store) : DiscreteEnergyField(domain, blob, num_class, class_weight, kernel_size, store)
{
    // nothing else to do
}

SampleEnergyField::~SampleEnergyField(void)
{
    // nothing else to do
}

int SampleEnergyField::Set(const vector<Sample> & samples)
{
    // samples only, no point
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if((samples[i].id < 0) || (samples[i].id >= NumClass()))
        {
            throw Exception("SampleEnergyField::Set(): illegal sample id");
            return 0;
        }
    }

    return DiscreteEnergyField::Set(samples);
}

int SampleEnergyField::Set(const vector<EnergySample> & samples)
{
    // samples only, no point
    Sample sample;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(! samples[i].GetSample(sample))
        {
            throw Exception("SampleEnergyField::Set(): cannot get sample");
            return 0;
        }

        if((sample.id < 0) || (sample.id >= NumClass()))
        {
            throw Exception("SampleEnergyField::Set(): illegal sample id");
            return 0;
        }
    }

    return DiscreteEnergyField::Set(samples);
}

int SampleEnergyField::Get(vector<Sample> & samples) const
{
    return DiscreteEnergyField::Get(samples);
}

int SampleEnergyField::Get(vector<EnergySample> & samples) const
{
    return DiscreteEnergyField::Get(samples);
}

const vector<const EnergySample *> & SampleEnergyField::Get(void) const
{
    return DiscreteEnergyField::Get();
}

const vector<EnergySample *> & SampleEnergyField::Get(void)
{
    return DiscreteEnergyField::Get();
}

float SampleEnergyField::Get(const EnergySample & query) const
{
    return DiscreteEnergyField::Get(query);
}

const EnergySample * SampleEnergyField::Peak(const bool including_fixed) const
{
    const vector<const EnergySample *> & samples = const_cast<const EnergySampleStore &>(_store).Get();

    return Peak(samples, including_fixed);
}

const EnergySample * SampleEnergyField::Peak(const EnergySample & query, const bool including_fixed) const
{
    return DiscreteEnergyField::Peak(query, including_fixed);
}

const EnergySample * SampleEnergyField::Peak(const vector<const EnergySample *> & samples, const bool including_fixed) const
{
    float max_energy = -Math::INF;
    float energy = 0;
    const EnergySample * peak = 0;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!samples[i] || (!samples[i]->GetValue(energy)))
        {
            throw Exception("SampleEnergyField::Peak(): cannot get sample energy");
            return 0;
        }

        if(!including_fixed && samples[i]->Fixed()) continue;

        if(energy > max_energy)
        {
            max_energy = energy;
            peak = samples[i];
        }
    }

    return peak;
}
