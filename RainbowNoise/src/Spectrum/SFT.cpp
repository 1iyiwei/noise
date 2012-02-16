/*
  SFT.cpp

  Li-Yi Wei
  08/21/2008

*/

#include "SFT.hpp"
#include <math.h>

SFT::SFT(const vector<Sample> & samples) : PFT(samples)
{
    // nothing to do
}

SFT::SFT(void)
{
    // nothing to do
}

SFT::~SFT(void)
{
    // nothing to do
}

float SFT::Evaluate(const Coordinate & w) const
{
    Value answer(0, 0);

    for(unsigned int i = 0; i < _samples.size(); i++)
    {
        if(w.Dimension() != _samples[i].coordinate.Dimension())
        {
            // error
            answer = Value(0, 0);
            break;
        }

        float foo = 0;

        for(int j = 0; j < w.Dimension(); j++)
        {
            foo += w[j]*_samples[i].coordinate[j];
        }
        
        answer.r += _samples[i].value * cos(foo);
        answer.i += _samples[i].value * sin(foo);
    }

    return (answer.r*answer.r + answer.i*answer.i);
}
