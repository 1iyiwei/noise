/*
  TrackerTroughFinder.cpp

  Li-Yi Wei
  09/04/2009

*/

#include "TrackerTroughFinder.hpp"
#include "Math.hpp"
#include "Exception.hpp"

TrackerTroughFinder::TrackerTroughFinder(const TrackerEnergyField & energy_field, const int num_trials, const bool greedy) : _energy_field(energy_field), _num_trials(num_trials), _greedy(greedy)
{
    // nothing else to do
}

TrackerTroughFinder::~TrackerTroughFinder(void)
{
    // nothing else to do
}

int TrackerTroughFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    if(&energy_field != &_energy_field)
    {
        throw Exception("TrackerTroughFinder::Find(): pointer mismatch");
        return 0;
    }

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
        _sample.id = answer.id;
        if(!_energy_field.Trough(_sample))
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
