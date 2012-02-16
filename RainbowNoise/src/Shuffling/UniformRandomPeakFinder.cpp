/*
  UniformRandomPeakFinder.cpp

  Li-Yi Wei
  09/06/2009

*/

#include <math.h>

#include "UniformRandomPeakFinder.hpp"
#include "Random.hpp"
#include "Exception.hpp"

UniformRandomPeakFinder::UniformRandomPeakFinder(void)
{
    // nothing to do
}

UniformRandomPeakFinder::~UniformRandomPeakFinder(void)
{
    // nothing to do
}

const EnergySample * UniformRandomPeakFinder::Find(const EnergyField & energy_field) const
{
    const vector<const EnergySample *> & samples = energy_field.Get();

    if(samples.size() <= 0)
    {
        return 0;
    }

    const int selection = static_cast<int>(floor(Random::UniformRandom()*samples.size()))%samples.size();

    return samples[selection];
}

int UniformRandomPeakFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    return 0;
}
