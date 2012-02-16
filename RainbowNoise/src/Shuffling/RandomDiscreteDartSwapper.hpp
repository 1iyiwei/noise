/*
  RandomDiscreteDartSwapper.hpp

  simply picks pairs of samples (with different class) to swap randomly

  Li-Yi Wei
  08/17/2009

*/

#ifndef _RANDOM_DISCRETE_DART_SWAPPER_HPP
#define _RANDOM_DISCRETE_DART_SWAPPER_HPP

#include <vector>
using namespace std;

#include "DiscreteDartSwapper.hpp"

class RandomDiscreteDartSwapper : public DiscreteDartSwapper
{
public:
    RandomDiscreteDartSwapper(EnergyField & energy_field);
    virtual ~RandomDiscreteDartSwapper(void);

    float Next(void);

protected:
    Sample _sample0, _sample1;
};

#endif
