/*
  LFT.cpp

  Li-Yi Wei
  08/21/2008

*/

#include "LFT.hpp"
#include <math.h>

LFT::LFT(const vector<Sample> & samples) : PFT(samples)
{
    // nothing to do
}

LFT::LFT(void)
{
    // nothing to do
}

LFT::~LFT(void)
{
    // nothing to do
}

float LFT::Evaluate(const Coordinate & w) const
{    
    float answer = 0;
    for(unsigned int i = 0; i < _samples.size(); i++)
        for(unsigned int j = 0; j < _samples.size(); j++)
        {
            if((w.Dimension() != _samples[i].coordinate.Dimension()) || (w.Dimension() != _samples[j].coordinate.Dimension()))
            {
                // error
                answer = 0;
                break;
            }

            float foo = 0;

            for(int k = 0; k < w.Dimension(); k++)
            {
                foo += w[k]*(_samples[i].coordinate[k] - _samples[j].coordinate[k]);
            }
        
            answer += cos(foo)*_samples[i].value*_samples[j].value;
        }

    return (answer);
}
