/*
  EnergyField.cpp

  Li-Yi Wei
  06/10/2009

*/

#include "EnergyField.hpp"
#include "Math.hpp"
#include "Exception.hpp"

EnergyField::EnergyField(const Domain & domain, const SingleBlob & blob, const int num_class, const ClassWeight & class_weight, const KernelSize & kernel_size): _domain(domain), _num_class(num_class), _blob(blob, num_class, class_weight, kernel_size)
{
    // nothing else to do
}

EnergyField::~EnergyField(void)
{
    // nothing else to do
}

int EnergyField::Set(const vector<Sample> & samples)
{
    throw Exception("EnergyField::Set() shouldn't be called");
    return 0;
}

int EnergyField::Set(const vector<EnergySample> & samples)
{
    throw Exception("EnergyField::Set() shouldn't be called");
    return 0;
}

int EnergyField::Get(vector<Sample> & samples) const
{
    throw Exception("EnergyField::Get() shouldn't be called");
    return 0;
}

int EnergyField::Get(vector<EnergySample> & samples) const
{
    throw Exception("EnergyField::Get() shouldn't be called");
    return 0;
}

const vector<const EnergySample *> & EnergyField::Get(void) const
{
    throw Exception("EnergyField::Get() shouldn't be called");
    return _const_empty;
}

const vector<EnergySample *> & EnergyField::Get(void)
{
    throw Exception("EnergyField::Get() shouldn't be called");
    return _empty;
}

float EnergyField::Get(const Sample & query) const
{
    _query.SetSample(query);

    return Get(_query);
}

float EnergyField::Get(const EnergySample & query) const
{
    throw Exception("EnergyField::Get() shouldn't be called");
    return Math::NAN;
}

const EnergySample * EnergyField::Peak(const bool including_fixed) const
{
    throw Exception("EnergyField::Peak(const bool including_fixed): shouldn't be called");
    return 0;
}

const EnergySample * EnergyField::Trough(const EnergySample & peak) const
{
    return 0;
}

const EnergySample * EnergyField::Peak(const EnergySample & query, const bool including_fixed) const
{
    throw Exception("EnergyField::Peak(const EnergySample &, const bool including_fixed): shouldn't be called");
    return 0;
}

float EnergyField::EnergyPlus(const EnergySample & source)
{
    return EnergyMove(source, +1);
}

float EnergyField::EnergyMinus(const EnergySample & source)
{
    return EnergyMove(source, -1);
}

float EnergyField::Move(EnergySample & source, const Sample & target)
{
    throw Exception("EnergyField::Move(): shouldn't be called");
    return Math::NAN;
}

void EnergyField::Visible(EnergySample & sample, const bool value)
{
    sample.Visible(value);
}

float EnergyField::EnergyMove(const EnergySample & source, const int sign)
{
    throw Exception("EnergyField::EnergyMove(): shouldn't be called");
    return Math::NAN;
}

const Domain & EnergyField::GetDomain(void) const
{
    return _domain;
}

int EnergyField::NumClass(void) const
{
    return _num_class;
}

float EnergyField::Get(const Sample & donor, const Sample & receiver) const
{
    throw Exception("EnergyField::Get(): shouldn't be called");
    return Math::NAN;
}
