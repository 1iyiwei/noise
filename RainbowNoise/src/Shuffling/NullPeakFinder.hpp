/*
  NullPeakFinder.hpp

  doing nothing, but a non-virtual class for null peak finder

  Li-Yi Wei
  06/18/2009

*/

#ifndef _NULL_PEAK_FINDER_HPP
#define _NULL_PEAK_FINDER_HPP

#include "PeakFinder.hpp"

class NullPeakFinder : public PeakFinder
{
public:
    NullPeakFinder(void);
    ~NullPeakFinder(void);

    const EnergySample * Find(const EnergyField & energy_field) const;
    int Find(const EnergyField & energy_field, Sample & answer) const;
};

#endif
