/*
  UniformWhiteNoise.cpp

  Li-Yi Wei
  09/01/2010

*/

#include "UniformWhiteNoise.hpp"
#include "Random.hpp"
#include "Exception.hpp"

UniformWhiteNoise::UniformWhiteNoise(const vector<float> & domain_size): WhiteNoise(domain_size)
{
    if(! Set(domain_size)) throw Exception("UniformWhiteNoise::UniformWhiteNoise(): error in setting domain size");
}

UniformWhiteNoise::~UniformWhiteNoise(void)
{
    // nothing else to do
}

int UniformWhiteNoise::Get(Sample & sample) const
{
    if(sample.coordinate.Dimension() != _domain_size.size())
    {
        sample.coordinate = Coordinate(_domain_size.size());
    }

    for(int i = 0; i < sample.coordinate.Dimension(); i++)
    {
        sample.coordinate[i] = Random::UniformRandom()*_domain_size[i];
    }
    
    sample.id = 0;
    sample.value = 1.0;
    
    // done
    return 1;
}

int UniformWhiteNoise::Set(const vector<float> & domain_size)
{
    // only handle square domain now
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        if(domain_size[0] != domain_size[i])
        {
            return 0;
        }
    }

    return 1;
}
