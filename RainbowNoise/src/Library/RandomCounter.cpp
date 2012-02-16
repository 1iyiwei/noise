/*
  RandomCounter.cpp

  Li-Yi Wei
  08/02/2007

*/

#include <math.h>

#include "RandomCounter.hpp"
#include "Random.hpp"

RandomCounter::RandomCounter(const int dimension, const int digit_min, const int digit_max) : Counter(dimension), _counter(dimension, digit_min, digit_max), _index(0)
{
    // nothing to do
}

RandomCounter::RandomCounter(const int dimension, const vector<int> & digit_min, const vector<int> & digit_max) : Counter(dimension), _counter(dimension, digit_min, digit_max), _index(0)
{
    // nothing to do
}

RandomCounter::~RandomCounter(void)
{
    // nothing to do
}

int RandomCounter::Reset(void)
{
    _counter.Reset();
    _values.clear();

    vector<int> value;

    do
    {
        _counter.Get(value);
        _values.push_back(value);
    }
    while(_counter.Next());

    _index = 0;

    // randomize
    vector< vector<int> > _random_values;

    while(_values.size() > 0)
    {
        const int selection = static_cast<int>(floor(Random::UniformRandom()*(_values.size()-1)));

        _random_values.push_back(_values[selection]);
        _values[selection] = _values[_values.size()-1];
        _values.pop_back();
    }

    _values = _random_values;

    return 1;
}

int RandomCounter::Get(vector<int> & value) const
{
    if((_index >= 0) && (_index < _values.size()))
    {
        value = _values[_index];
        return 1;
    }
    else
    {
        return 0;
    }
}

int RandomCounter::Next(void)
{
    if(_index < (_values.size()-1))
    {
        _index++;
        return 1;
    }
    else
    {
        return 0;
    }
}
