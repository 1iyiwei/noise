/*
  BagEnergySampleStore.hpp

  the most basic energy sample store without any structure

  Li-Yi Wei
  07/08/2009

*/

#ifndef _BAG_ENERGY_SAMPLE_STORE_HPP
#define _BAG_ENERGY_SAMPLE_STORE_HPP

#include "EnergySampleStore.hpp"

class BagEnergySampleStore : public EnergySampleStore
{
public:
    BagEnergySampleStore(void);
    ~BagEnergySampleStore(void);

    BagEnergySampleStore * Clone(void) const;

    // all functions directly from parent class
};

#endif
