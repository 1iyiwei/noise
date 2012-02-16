/*
  DiscreteTroughFinder.hpp

  finding troughs via a predetermined set of points

  a dummy class that essentially uses TribePointEnergyField

  Li-Yi Wei
  08/21/2009

*/

#ifndef _DISCRETE_TROUGH_FINDER_HPP
#define _DISCRETE_TROUGH_FINDER_HPP

#include "TroughFinder.hpp"
#include "TribePointEnergyField.hpp"

class DiscreteTroughFinder : public TroughFinder
{
public:
    DiscreteTroughFinder(const TribePointEnergyField & energy_field);
    ~DiscreteTroughFinder(void);

    int Find(const EnergyField & energy_field, Sample & answer) const;

protected:
    const TribePointEnergyField & _energy_field;
};

#endif
