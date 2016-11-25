/*
  SampleHolder.cpp

  Li-Yi Wei
  09/21/2010

*/

#include "SampleHolder.hpp"
#include "Exception.hpp"

SampleHolder::SampleHolder(const Domain & domain): _domain(domain)
{
    // nothing else to do
}

SampleHolder::~SampleHolder(void)
{
    // nothing else to do
}

const Domain & SampleHolder::GetDomain(void) const
{
    return _domain;
}

int SampleHolder::Inside(const Sample & sample) const
{
    throw Exception("SampleHolder::Inside(): shouldn't be called");
    return 0;
}

int SampleHolder::Add(const Sample & sample)
{
    throw Exception("SampleHolder::Add(): shouldn't be called");
    return 0;
}

int SampleHolder::Remove(const Sample & sample)
{
    throw Exception("SampleHolder::Remove(): shouldn't be called");
    return 0;
}

void SampleHolder::Clear(void)
{
    throw Exception("SampleHolder::Clear(): shouldn't be called");
}

void SampleHolder::GetSamples(vector<const Sample *> & samples) const
{
    throw Exception("SampleHolder::GetSamples(): shouldn't be called");
}

int SampleHolder::GetNeighbors(const Sample & query, const float radius, vector<const Sample *> & neighbors) const
{
    throw Exception("SampleHolder::GetNeighbors(): shouldn't be called");

    return 0;
}
