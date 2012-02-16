/*
  Difference Fourier Transform

  a subclass of PFT, 
  computing directly from location differences instead of samples
  (and thus allows theoretical study for fictitious location differences)

  Li-Yi Wei
  10/30/2008

*/

#ifndef _DFT_HPP
#define _DFT_HPP

#include <vector>
using namespace std;

#include "PFT.hpp"

class DFT : public PFT
{
public:
    DFT(const vector<Sample> & differences);
    DFT(void);
    ~DFT(void);

    float Evaluate(const Coordinate & w) const;
};

#endif
