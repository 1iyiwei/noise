/*
  DiscreteDartSwapper.hpp

  the base class for all kinds of dart swapper
  operating on a discrete set of points

  Li-Yi Wei
  08/17/2009

*/

#ifndef _DISCRETE_DART_SWAPPER_HPP
#define _DISCRETE_DART_SWAPPER_HPP

#include <vector>
using namespace std;

#include "DartSwapper.hpp"
#include "Sample.hpp"

class DiscreteDartSwapper : public DartSwapper
{
public:
    DiscreteDartSwapper(EnergyField & energy_field);
    virtual ~DiscreteDartSwapper(void) = 0;

    // initialization with samples + points
    // points are indicated with negative (class) id
    virtual int Set(const vector<Sample> & samples_and_points);

    // get the sample + points set
    virtual int Get(vector<Sample> & samples_and_points) const;

    // next iteration
    virtual float Next(void);

protected:
    // assuming both already belong to the energy sample field
    // return energy value change
    float Swap(EnergySample & s0, EnergySample & s1);
    mutable Sample _s0, _s1;
};

#endif
