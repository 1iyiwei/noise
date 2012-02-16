/*
  EnergySample.cpp

  Li-Yi Wei
  07/07/2009

*/

#include "EnergySample.hpp"
#include "Math.hpp"

const float EnergySample::_DEFAULT_ENERGY_VALUE = 0; // Math::NAN

EnergySample::EnergySample(void): _value(_DEFAULT_ENERGY_VALUE), _empty(1), _visibility(1), _fixed(0)
{
    // nothing else to do
}

EnergySample::EnergySample(const int num_class): _value(_DEFAULT_ENERGY_VALUE), _values(num_class, _DEFAULT_ENERGY_VALUE), _empty(1), _visibility(1), _fixed(0)
{
    // nothing else to do
}

EnergySample::~EnergySample(void)
{
    // nothing else to do
}

int EnergySample::NumClass(void) const
{
    return _values.size();
}

int EnergySample::NumClass(const int num_class)
{
    _values = vector<float>(num_class, _DEFAULT_ENERGY_VALUE);

    return (num_class > 0);
}

int EnergySample::GetSample(Sample & sample) const
{
    if(! _empty)
    {
        sample = _sample;
    }

    return !_empty;
}

int EnergySample::SetSample(const Sample & sample)
{
    _sample = sample;
#if 0 // hmm, I see no reason to clear energy value
#ifdef _ENERGY_MULTI_VALUE
    _values = vector<float>(_value.size(), _DEFAULT_ENERGY_VALUE);
#else
    _value = _DEFAULT_ENERGY_VALUE;
#endif
#endif
    _empty = 0;

    return 1;
}

int EnergySample::SetSample(void)
{
#if 0
#ifdef _ENERGY_MULTI_VALUE
    _values = vector<float>(_value.size(), _DEFAULT_ENERGY_VALUE);
#else
    _value = _DEFAULT_ENERGY_VALUE;
#endif
#endif
    _empty = 1;
    return 1;
}

float EnergySample::GetValue(const int class_id) const
{
    if((class_id < 0) || (class_id >= _values.size()) || (_empty))
    {
        return _DEFAULT_ENERGY_VALUE;
    }
    else
    {
        return _values[class_id];
    }
}

int EnergySample::GetValue(const int class_id, float & value) const
{
    if((class_id < 0) || (class_id >= _values.size()))
    {
        return 0;
    }

    if(! _empty)
    {
        value = _values[class_id];
    }

    return !_empty;
}

float EnergySample::GetValue(void) const
{
    if(_empty)
    {
        return _DEFAULT_ENERGY_VALUE;
    }
    else
    {
        return _value;
    }
}

int EnergySample::GetValue(float & value) const
{
    if(! _empty)
    {
        value = _value;
    }

    return !_empty;
}

int EnergySample::SetValue(const int class_id, const float value)
{
    if((class_id < 0) || (class_id >= _values.size()))
    {
        return 0;
    }

    if(! _empty)
    {
        _values[class_id] = value;
    }

    return !_empty;
}

int EnergySample::SetValue(const float value)
{
    if(! _empty)
    {
        _value = value;
    }

    return !_empty;
}

bool EnergySample::Visible(void) const
{
    return _visibility;
}

void EnergySample::Visible(const bool value)
{
    _visibility = value;
}

bool EnergySample::Fixed(void) const
{
    return _fixed;
}

void EnergySample::Fixed(const bool value)
{
    _fixed = value;
}
