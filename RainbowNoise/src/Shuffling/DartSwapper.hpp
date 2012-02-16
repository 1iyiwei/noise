/*
  DartSwapper.hpp

  the base class for all kinds of dart swapper

  Li-Yi Wei
  08/17/2009

*/

#ifndef _DART_SWAPPER_HPP
#define _DART_SWAPPER_HPP

#include "EnergyField.hpp"

class DartSwapper
{
public:
    DartSwapper(EnergyField & energy_field);
    virtual ~DartSwapper(void) = 0;

    // initialization
    virtual int Set(const vector<Sample> & samples);

    // get the sample set
    virtual int Get(vector<Sample> & samples) const;

    // next iteration
    // return the change of energy value
    virtual float Next(void);

protected:
    EnergyField & _energy_field;
};

#endif
