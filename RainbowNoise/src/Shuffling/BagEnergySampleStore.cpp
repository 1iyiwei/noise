/*
  BagEnergySampleStore.cpp

  Li-Yi Wei
  07/08/2009

*/

#include "BagEnergySampleStore.hpp"

BagEnergySampleStore::BagEnergySampleStore(void)
{
    // nothing to do
}

BagEnergySampleStore::~BagEnergySampleStore(void)
{
    // nothing to do
}
 
BagEnergySampleStore * BagEnergySampleStore::Clone(void) const
{
    return new BagEnergySampleStore(*this);
}
