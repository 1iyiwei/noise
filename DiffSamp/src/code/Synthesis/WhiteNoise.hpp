/*
  WhiteNoise.hpp

  the base class for all white noise sample generator
  
  Li-Yi Wei
  09/01/2010

*/

#ifndef _WHITE_NOISE_HPP
#define _WHITE_NOISE_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"

class WhiteNoise
{
public:
    WhiteNoise(const vector<float> & domain_size);
    virtual ~WhiteNoise(void) = 0;

    virtual int Get(Sample & output) const;
    virtual int Set(const vector<float> & domain_size);

protected:
    vector<float> _domain_size;
};

#endif
