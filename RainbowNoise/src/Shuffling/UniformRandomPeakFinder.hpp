/*
  UniformRandomPeakFinder.hpp

  doing nothing, but a non-virtual class for null peak finder

  Li-Yi Wei
  09/06/2009

*/

#ifndef _UNIFORM_RANDOM_PEAK_FINDER_HPP
#define _UNIFORM_RANDOM_PEAK_FINDER_HPP

#include "PeakFinder.hpp"

class UniformRandomPeakFinder : public PeakFinder
{
public:
    UniformRandomPeakFinder(void);
    ~UniformRandomPeakFinder(void);

    const EnergySample * Find(const EnergyField & energy_field) const;
    int Find(const EnergyField & energy_field, Sample & answer) const;
};

#endif
