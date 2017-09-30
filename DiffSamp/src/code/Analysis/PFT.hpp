/*
  Power Fourier Transform

  the base class for computing power (magnitude^2) of sample fourier spectrums
  
  Li-Yi Wei
  08/21/2008

*/

#ifndef _PFT_HPP
#define _PFT_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"

class PFT
{
public:
    PFT(void);
    virtual ~PFT(void) = 0;

    // evaluation in the angular frequency w and returns power |Fourier(w)|^2
    virtual float Evaluate(const Coordinate & w) const;
   
protected:
    
    struct Value
    {
        Value(void);
        Value(const float r, const float i);
        
        // real and imaginary components
        float r, i;
    };
};

#endif

