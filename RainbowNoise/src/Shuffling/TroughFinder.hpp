/*
  TroughFinder.hpp

  the base class for all trough finders

  Li-Yi Wei
  06/10/2009

*/

#ifndef _TROUGH_FINDER_HPP
#define _TROUGH_FINDER_HPP

#include "EnergyField.hpp"

class TroughFinder
{
public:
    TroughFinder(void);
    virtual ~TroughFinder(void) = 0;

    // answer initialized as the peak found
    virtual int Find(const EnergyField & energy_field, Sample & answer) const;

    // set the ceiling energy threshold so the computation stops
    // when finding a trough with energy <= this value
    void ThresholdEnergy(const float threshold_energy);

protected:
    float _threshold_energy;
};

#endif
