/*
  PeakFinder.hpp

  the base class for all peak finders

  Li-Yi Wei
  06/10/2009

*/

#ifndef _PEAK_FINDER_HPP
#define _PEAK_FINDER_HPP

#include "EnergyField.hpp"

class PeakFinder
{
public:
    virtual ~PeakFinder(void) = 0;

    virtual const EnergySample * Find(const EnergyField & energy_field) const;
    virtual int Find(const EnergyField & energy_field, Sample & answer) const;
};

#endif
