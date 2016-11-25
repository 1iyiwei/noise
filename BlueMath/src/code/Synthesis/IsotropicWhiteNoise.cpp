/*
  IsotropicWhiteNoise.cpp

  Li-Yi Wei
  09/01/2010

*/

#include "IsotropicWhiteNoise.hpp"
#include "Random.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

IsotropicWhiteNoise::IsotropicWhiteNoise(const vector<float> & domain_size, const Array<float> & importance): WhiteNoise(domain_size), _importance(importance), _importance_max(0), _uniform_white_noise(domain_size)
{
    const string message = Utility::Normalize(_importance, domain_size);

    if(message != "") throw Exception("IsotropicWhiteNoise::IsotropicWhiteNoise(): cannot normalize importance!");

    _importance.Size(_importance_size);

    // find max
    _importance_max = 0;
    vector<float> values;
    _importance.Get(values);

    for(unsigned int i = 0; i < values.size(); i++)
    {
        if(values[i] > _importance_max)
        {
            _importance_max = values[i];
        }
    }
}

IsotropicWhiteNoise::~IsotropicWhiteNoise(void)
{
    // nothing else to do
}

int IsotropicWhiteNoise::Get(Sample & sample) const
{
    bool done = 0;
    vector<int> index(_importance_size);
    float value = 0;

    do
    {
        if(! _uniform_white_noise.Get(sample)) return 0;

        if(sample.coordinate.Dimension() != _importance_size.size()) throw Exception("IsotropicWhiteNoise::Get(): sample.coordinate.Dimension() != _importance_size.size()");

        for(unsigned int i = 0; i < index.size(); i++)
        {
            index[i] = static_cast<int>(floor(sample.coordinate[i]/_domain_size[i]*_importance_size[i]));

            if(index[i] < 0) index[i] = 0;
            if(index[i] >= _importance_size[i]) index[i] = _importance_size[i]-1;
        }

        if(! _importance.Get(index, value))
        {
            throw Exception("IsotropicWhiteNoise::Get(): cannot get importance value");
            return 0;
        }
    }
    while(Random::UniformRandom()*_importance_max > value);

    // done
    return 1;
}
