/*
  UniformSphereRandomTroughFinder.hpp

  finding troughs via uniform random samples
  that lie within a distance from the query

  Li-Yi Wei
  09/06/2009

*/

#ifndef _UNIFORM_SPHERE_RANDOM_TROUGH_FINDER_HPP
#define _UNIFORM_SPHERE_RANDOM_TROUGH_FINDER_HPP

#include <vector>
using namespace std;

#include "TroughFinder.hpp"

class UniformSphereRandomTroughFinder : public TroughFinder
{
public:
    // greedy: return the first found trough that has energy lower than peak
    // instead of going through all num_trials
    UniformSphereRandomTroughFinder(const int num_class, const int num_trials, const bool greedy);
    UniformSphereRandomTroughFinder(const int num_class, const int num_trials, const bool greedy, const vector<float> & radius);
    ~UniformSphereRandomTroughFinder(void);

    int SetRadius(const vector<float> & radius);
    int Find(const EnergyField & energy_field, Sample & answer) const;

protected:
    static int SphereRandom(const Domain & domain, const Sample & query, const float radius, Sample & answer);

protected:
    const int _num_class;
    const int _num_trials;
    const bool _greedy;
    vector<float> _radius;
    mutable Sample _query, _sample;
};

#endif
