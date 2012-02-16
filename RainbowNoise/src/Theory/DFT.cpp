/*
  DFT.cpp

  Li-Yi Wei
  10/30/2008

*/

#include "DFT.hpp"
#include <math.h>

DFT::DFT(const vector<Sample> & differences) : PFT(differences)
{
    // nothing to do
}

DFT::DFT(void)
{
    // nothing to do
}

DFT::~DFT(void)
{
    // nothing to do
}

float DFT::Evaluate(const Coordinate & w) const
{
    float answer = 0;

    for(unsigned int i = 0; i < _samples.size(); i++)
    {
        if(w.Dimension() != _samples[i].coordinate.Dimension())
        {
            // error
            answer = 0;
            break;
        }

        float foo = 0;

        for(int j = 0; j < w.Dimension(); j++)
        {
            foo += w[j]*_samples[i].coordinate[j];
        }
        
        answer += _samples[i].value * _samples[i].value * (cos(foo) + 1.0);
    }

    return answer;
}
