/*
  WhiteNoise.cpp

  Li-Yi Wei
  09/01/2010

*/

#include "WhiteNoise.hpp"
#include "Exception.hpp"

WhiteNoise::WhiteNoise(const vector<float> & domain_size): _domain_size(domain_size)
{
    // nothing else to do
}

WhiteNoise::~WhiteNoise(void)
{
    // nothing else to do
}

int WhiteNoise::Get(Sample & output) const
{
    throw Exception("WhiteNoise::Get(): shouldn't be called");
    return 0;
}

int WhiteNoise::Set(const vector<float> & domain_size)
{
    _domain_size = domain_size;

    return 1;
}
