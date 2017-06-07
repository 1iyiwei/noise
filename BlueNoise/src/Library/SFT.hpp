/*
  Sample Fourier Transform

  Given a collection of sample points with float positions
  Compute the corresponding Fourier Transform

  Li-Yi Wei
  06/15/2007 - 2D version
  07/08/2008 - arbitrary dimension version
*/

#ifndef _SFT_HPP
#define _SFT_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"

class SFT
{
public:
    struct Value
    {
        Value(void);
        Value(const float r, const float i);
        
        // real and imaginary components
        float r, i;
    };
    
    SFT(const vector<Sample> & samples);
    SFT(void);
    ~SFT(void);

    // evaluation in the frequency w and returns F(wx, wy)
    Value Evaluate(const Coordinate & w) const;
    
protected:
    vector<Sample> _samples;
};

#endif
