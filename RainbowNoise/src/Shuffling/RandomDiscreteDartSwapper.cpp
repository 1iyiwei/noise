/*
  RandomDiscreteDartSwapper.cpp

  Li-Yi Wei
  08/17/2009

*/

#include "RandomDiscreteDartSwapper.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Exception.hpp"

RandomDiscreteDartSwapper::RandomDiscreteDartSwapper(EnergyField & energy_field) : DiscreteDartSwapper(energy_field)
{
    // nothing else to do
}

RandomDiscreteDartSwapper::~RandomDiscreteDartSwapper(void)
{
    // nothing else to do
}

float RandomDiscreteDartSwapper::Next(void)
{
    const vector<EnergySample *> & samples = _energy_field.Get();

    EnergySample * ps0 = 0;
    EnergySample * ps1 = 0;

    do
    {
        const int index0 = static_cast<int>(Random::UniformRandom()*samples.size())%samples.size();

        const int index1 = static_cast<int>(Random::UniformRandom()*samples.size())%samples.size();

        ps0 = samples[index0];
        ps1 = samples[index1];

        if(!ps0->GetSample(_sample0) || !ps1->GetSample(_sample1))
        {
            throw Exception("RandomDiscreteDartSwapper::Next(): cannot get samples");
            return Math::NAN;
        }
    }
    while(_sample0.id == _sample1.id);

    EnergySample & s0 = *ps0;
    EnergySample & s1 = *ps1;

    const float change = Swap(s0, s1);

    if(change < 0)
    {
        return change;
    }
    else
    {
        // swap back and make no changes
        Swap(s0, s1);
        return 0;
    }
}
