/*
  Linear Local Fourier Transform

  the O(N) acceleration of the original O(N^2) implementation of LFT

  Li-Yi Wei
  08/22/2008

*/

#ifndef _LINEAR_LFT_HPP
#define _LINEAR_LFT_HPP

#include <vector>
using namespace std;

#include "PFT.hpp"

class LinearLFT : public PFT
{
public:
    LinearLFT(const vector<Sample> & samples);
    LinearLFT(void);
    ~LinearLFT(void);

    float Evaluate(const Coordinate & w) const;

protected:
    vector<int> _order;
};

#endif

