/*
  Local Fourier Transform

  should return the same (up to floating point precision) result to SFT
  evaluates the basic formulation in my paper
  
  Li-Yi Wei
  08/21/2008

*/

#ifndef _LFT_HPP
#define _LFT_HPP

#include <vector>
using namespace std;

#include "PFT.hpp"

class LFT : public PFT
{
public:
    LFT(const vector<Sample> & samples);
    LFT(void);
    ~LFT(void);

    float Evaluate(const Coordinate & w) const;
};

#endif

