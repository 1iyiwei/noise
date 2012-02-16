/*
  NullTroughFinder.hpp

  doing nothing, but a non-virtual class for null trough finder

  Li-Yi Wei
  08/11/2009

*/

#ifndef _NULL_TROUGH_FINDER_HPP
#define _NULL_TROUGH_FINDER_HPP

#include "TroughFinder.hpp"

class NullTroughFinder : public TroughFinder
{
public:
    NullTroughFinder(void);
    ~NullTroughFinder(void);

    int Find(const EnergyField & energy_field, Sample & answer) const;
};

#endif
