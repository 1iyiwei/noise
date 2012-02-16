/*
  TrackerTroughFinder.hpp

  a dummy class that essentially uses TrackerEnergyField

  Li-Yi Wei
  09/04/2009

*/

#ifndef _TRACKER_TROUGH_FINDER_HPP
#define _TRACKER_TROUGH_FINDER_HPP

#include "TroughFinder.hpp"
#include "TrackerEnergyField.hpp"

class TrackerTroughFinder : public TroughFinder
{
public:
    TrackerTroughFinder(const TrackerEnergyField & energy_field, const int num_trials, const bool greedy);
    ~TrackerTroughFinder(void);

    int Find(const EnergyField & energy_field, Sample & answer) const;

protected:
    const TrackerEnergyField & _energy_field;
    const int _num_trials;
    const bool _greedy;
    mutable Sample _sample;
};

#endif
