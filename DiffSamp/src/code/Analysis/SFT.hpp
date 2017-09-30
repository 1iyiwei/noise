/*
  Sample Fourier Transform

  Li-Yi Wei
  08/21/2008

*/

#ifndef _SFT_HPP
#define _SFT_HPP

#include <vector>
using namespace std;

#include "PFT.hpp"

class SFT : public PFT
{
public:
    SFT(const vector<Sample> & samples);
    SFT(void);
    ~SFT(void);

    float Evaluate(const Coordinate & w) const;

protected:
    vector<Sample> _samples;
};

#endif

