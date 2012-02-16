/*
  DiffGenerator.hpp

  the base class for all location-difference-pair generators

  Li-Yi Wei
  10/30/2008

*/

#ifndef _DIFF_GENERATOR_HPP
#define _DIFF_GENERATOR_HPP

#include "DFT.hpp"

class DiffGenerator
{
public:
    DiffGenerator(const vector<float> & domain_size);
    virtual ~DiffGenerator(void) = 0;

    // the initial size of differences indicates the desired # of results
    // return 1 if success, 0 else
    virtual int Produce(vector<Sample> & differences) const;

protected:
    const vector<float> _domain_size;
};

#endif

 
