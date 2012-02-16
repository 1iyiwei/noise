/*
  UniformSphereRandomTroughFinder.cpp

  Li-Yi Wei
  09/06/2009

*/

#include "UniformSphereRandomTroughFinder.hpp"
#include "Random.hpp"
#include "Math.hpp"
#include "Exception.hpp"

UniformSphereRandomTroughFinder::UniformSphereRandomTroughFinder(const int num_class, const int num_trials, const bool greedy) : _num_class(num_class), _num_trials(num_trials), _greedy(greedy), _radius(num_class, Math::INF)
{
    // nothing to do
}

UniformSphereRandomTroughFinder::UniformSphereRandomTroughFinder(const int num_class, const int num_trials, const bool greedy, const vector<float> & radius) : _num_class(num_class), _num_trials(num_trials), _greedy(greedy), _radius(radius)
{
    if(radius.size() != num_class)
    {
        throw Exception("UniformSphereRandomTroughFinder::UniformSphereRandomTroughFinder(): radius.size() != num_class");
    }
}

UniformSphereRandomTroughFinder::~UniformSphereRandomTroughFinder(void)
{
    // nothing to do
}

int UniformSphereRandomTroughFinder::SetRadius(const vector<float> & radius)
{
    _radius = radius;

    return (_radius.size() == _num_class);
}

int UniformSphereRandomTroughFinder::Find(const EnergyField & energy_field, Sample & answer) const
{
    if((answer.id < 0) || (answer.id >= _radius.size()))
    {
        throw Exception("UniformSphereRandomTroughFinder::Find(): illegal class id");
        return 0;
    }

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

    _query = answer;

    for(int i = 0; i < _num_trials; i++)
    {
        if(! SphereRandom(domain, _query, _radius[_query.id], _sample))
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

int UniformSphereRandomTroughFinder::SphereRandom(const Domain & domain, const Sample & query, const float radius, Sample & answer)
{
    answer = query;

    float distance2 = Math::INF;
   
    do
    {
        for(int i = 0; i < answer.coordinate.Dimension(); i++)
        {
            answer.coordinate[i] = Random::UniformRandom()*2-1;
        }

        distance2 = 0;
        for(int i = 0; i < answer.coordinate.Dimension(); i++)
        {
            distance2 += answer.coordinate[i]*answer.coordinate[i];
        }
    }
    while(distance2 > 1.0);

    for(int i = 0; i < answer.coordinate.Dimension(); i++)
    {
        answer.coordinate[i] = query.coordinate[i] + answer.coordinate[i]*radius;
    }

    // assuming toroidal domain for now
    // could be fixed by repeated trials
    if(! domain.Adopt(answer, answer))
    {
        throw Exception("UniformSphereRandomTroughFinder::SphereRandom(): cannot adopt sample back into domain");
        return 0;
    }

    // done
    return 1;
}
