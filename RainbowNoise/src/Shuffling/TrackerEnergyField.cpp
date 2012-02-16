/*
  TrackerEnergyField.cpp

  Li-Yi Wei
  09/04/2009

*/

#include "TrackerEnergyField.hpp"
#include "Math.hpp"
#include "Exception.hpp"
 
TrackerEnergyField::TrackerEnergyField(EnergyField & real_energy_field, EnergySampleTracker & tracker) : EnergyField(_dummy_domain, _dummy_blob, 0, _dummy_class_weight, _dummy_kernel_size), _energy_field(real_energy_field), _tracker(tracker)
{
    // nothing else to do
}

TrackerEnergyField::~TrackerEnergyField(void)
{
    // nothing else to do
}

int TrackerEnergyField::Set(const vector<Sample> & samples)
{
    if(!_energy_field.Set(samples))
    {
        return 0;
    }

    return LocalSet();
}

int TrackerEnergyField::Set(const vector<EnergySample> & samples)
{
    if(!_energy_field.Set(samples))
    {
        return 0;
    }

    return LocalSet();
}

int TrackerEnergyField::LocalSet(void)
{
    const vector<const EnergySample *> & energy_samples = const_cast<const EnergyField &>(_energy_field).Get();

    if(!_tracker.Set(energy_samples))
    {
        return 0;
    }

    return 1;
}

int TrackerEnergyField::Get(vector<Sample> & samples) const
{
    return _energy_field.Get(samples);
}

int TrackerEnergyField::Get(vector<EnergySample> & samples) const
{
    return _energy_field.Get(samples);
}

const vector<const EnergySample *> & TrackerEnergyField::Get(void) const
{
    return const_cast<const EnergyField &>(_energy_field).Get();
}

const vector<EnergySample *> & TrackerEnergyField::Get(void)
{
    return _energy_field.Get();
}

float TrackerEnergyField::Get(const Sample & query) const
{
    return _energy_field.Get(query);
}

float TrackerEnergyField::Get(const EnergySample & query) const
{
    return _energy_field.Get(query);
}

const EnergySample * TrackerEnergyField::Peak(const bool including_fixed) const
{
    return _energy_field.Peak(including_fixed);
}

const EnergySample * TrackerEnergyField::Trough(const EnergySample & peak) const
{
    return _energy_field.Trough(peak);
}

const EnergySample * TrackerEnergyField::Peak(const EnergySample & query, const bool including_fixed) const
{
    return _energy_field.Peak(query, including_fixed);
}

int TrackerEnergyField::Trough(Sample & sample) const
{
    return _tracker.Trough(sample);
}

float TrackerEnergyField::Move(EnergySample & source, const Sample & target)
{
    if(! _tracker.Minus(source))
    {
        throw Exception("TrackerEnergyField::Move(): cannot tracker minus");
        return Math::NAN;
    }

    const float value = _energy_field.Move(source, target);

    if(! _tracker.Plus(source))
    {
        throw Exception("TrackerEnergyField::Move(): cannot tracker plus");
        return Math::NAN;
    }

    return value;
}

float TrackerEnergyField::EnergyMove(const EnergySample & source, const int sign)
{
#if 0
    throw Exception("TrackerEnergyField::EnergyMove(): no need to touch tracker!");
    int status = 0;

    if(sign == 1)
    {
        status = _tracker.Plus(source);
    }
    else if(sign == -1)
    {
        status = _tracker.Minus(source);
    }
    else
    {
        throw Exception("TrackerEnergyField::EnergyMove(): unknown sign option");
        return Math::NAN;
    }

    if(! status)
    {
        throw Exception("TrackerEnergyField::EnergyMove(): cannot tracker plus/minus");
        return Math::NAN;
    }
#endif

    return _energy_field.EnergyMove(source, sign);
}

void TrackerEnergyField::Visible(EnergySample & sample, const bool value)
{
    if(!sample.Visible() && value)
    {
        if(! _tracker.Plus(sample))
        {
            throw Exception("TrackerEnergyField::Visible(): cannot tracker plus");
        }
    }
    else if(sample.Visible() & !value)
    {
        if(! _tracker.Minus(sample))
        {
            throw Exception("TrackerEnergyField::Visible(): cannot tracker minus");
        }
    }

    _energy_field.Visible(sample, value);
}

float TrackerEnergyField::Get(const Sample & donor, const Sample & receiver) const
{
    return _energy_field.Get(donor, receiver);
}

PlainBoxDomain TrackerEnergyField::_dummy_domain(vector<float>(1,1));
GaussianBlob TrackerEnergyField::_dummy_blob;
FairClassWeight TrackerEnergyField::_dummy_class_weight(vector<int>(1,1));
UniformFairKernelSize TrackerEnergyField::_dummy_kernel_size(vector<float>(1,1), vector<int>(1,1));
