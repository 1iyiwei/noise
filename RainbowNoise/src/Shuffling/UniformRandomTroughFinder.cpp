/*
  UniformRandomTroughFinder.cpp

  Li-Yi Wei
  06/18/2009

*/

#include "UniformRandomTroughFinder.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Exception.hpp"

UniformRandomTroughFinder::UniformRandomTroughFinder(const int num_trials, const bool greedy) : _num_trials(num_trials), _greedy(greedy)
{
    // nothing to do
}

UniformRandomTroughFinder::~UniformRandomTroughFinder(void)
{
    // nothing to do
}

int UniformRandomTroughFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    const Domain & domain = energy_field.GetDomain();

    float min_energy = Math::INF;
#if 1
    try
    {
        min_energy = energy_field.Get(answer);
    }
    catch(Exception e)
    {
        min_energy = Math::INF;
    }
#endif

    const float peak_energy = min_energy;

    const bool greedy = _greedy && (peak_energy < Math::INF);

    for(int i = 0; i < _num_trials; i++)
    {
        if(! domain.UniformRandom(_sample))
        {
            return 0;
        }

        _sample.id = answer.id;
        const float energy = energy_field.Get(_sample);

        if(energy < min_energy)
        {
            min_energy = energy;
            answer = _sample;
        }

        if(greedy && (energy < peak_energy))
        {
            break;
        }

        if(energy < _threshold_energy)
        {
            break;
        }
    }

    return (min_energy < Math::INF);
}
