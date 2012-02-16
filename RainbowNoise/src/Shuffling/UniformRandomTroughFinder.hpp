/*
  UniformRandomTroughFinder.hpp

  finding troughs via uniform random samples

  Li-Yi Wei
  06/18/2009

*/

#ifndef _UNIFORM_RANDOM_TROUGH_FINDER_HPP
#define _UNIFORM_RANDOM_TROUGH_FINDER_HPP

#include "TroughFinder.hpp"

class UniformRandomTroughFinder : public TroughFinder
{
public:
    // greedy: return the first found trough that has energy lower than peak
    // instead of going through all num_trials
    UniformRandomTroughFinder(const int num_trials, const bool greedy);
    ~UniformRandomTroughFinder(void);

    int Find(const EnergyField & energy_field, Sample & answer) const;

protected:
    const int _num_trials;
    const bool _greedy;
    mutable Sample _sample;
};

#endif
