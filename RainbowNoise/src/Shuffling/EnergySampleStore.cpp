/*
  EnergySampleStore.cpp

  Li-Yi Wei
  07/08/2009

*/

#include "EnergySampleStore.hpp"
#include "Exception.hpp"

EnergySampleStore::~EnergySampleStore(void)
{
    // nothing to do
}

EnergySampleStore * EnergySampleStore::Clone(void) const
{
    throw Exception("EnergySampleStore::Clone() shouldn't be called");
    return 0;
}

int EnergySampleStore::Set(const vector<EnergySample> & samples)
{
    _samples = samples;
    
    _const_p_samples = vector<const EnergySample *>(_samples.size());
    for(unsigned int i = 0; i < _const_p_samples.size(); i++)
    {
        _const_p_samples[i] = & _samples[i];
    }

    _p_samples = vector<EnergySample *>(_samples.size());
    for(unsigned int i = 0; i < _p_samples.size(); i++)
    {
        _p_samples[i] = & _samples[i];
    }

    return 1;
}

int EnergySampleStore::Get(vector<EnergySample> & samples) const
{
    samples = _samples;
    return 1;
}

const vector<const EnergySample *> & EnergySampleStore::Get(void) const
{
    return _const_p_samples;
}

const vector<EnergySample *> & EnergySampleStore::Get(void)
{
    return _p_samples;
}

const vector<const EnergySample *> & EnergySampleStore::Get(const Sample & query) const
{
    // return everyone as neighbor
    return _const_p_samples;
}

const vector<EnergySample *> & EnergySampleStore::Get(const Sample & query)
{
    // return everyone as neighbor
    return _p_samples;
}

int EnergySampleStore::Plus(const EnergySample & rookie)
{
    _samples.push_back(rookie);
    _const_p_samples.push_back(&_samples[_samples.size()-1]);
    _p_samples.push_back(&_samples[_samples.size()-1]);

    return 1;
}

int EnergySampleStore::Minus(const int index)
{
    if((index >= 0) && (index < _samples.size()))
    {
        _samples[index] = _samples[_samples.size()-1];
        _samples.pop_back();

        _const_p_samples[index] = &_samples[index];
        _const_p_samples.pop_back();

        _p_samples[index] = &_samples[index];
        _p_samples.pop_back();

        return 1;
    }
    else
    {
        return 0;
    }
}

int EnergySampleStore::Move(EnergySample & source, const Sample & target)
{
#if 0
    throw Exception("EnergySampleStore::Move(): error checking highly inefficient; should be performed in offspring class");

    // error check
    int index = -1;
    for(unsigned int i = 0; i < _const_p_samples.size(); i++)
    {
        if(_const_p_samples[i] == (&source))
        {
            index = i;
            break;
        }
    }

    if(index < 0)
    {
        throw Exception("EnergySampleStore::Move(): source not part of the store!");
        return 0;
    }
#endif

    if(source.GetSample(_sample) && (_sample.id != target.id))
    {
        throw Exception("EnergySampleStore::Move(): cannot change sample class");
        return 0;
    }

    return source.SetSample(target);
}
