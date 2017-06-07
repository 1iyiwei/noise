/*
  SFT.cpp

  Li-Yi Wei
  06/15/2007 - 2D
  07/08/2007 - arbitrary D

*/

#include "SFT.hpp"
#include <math.h>

SFT::Value::Value(void) : r(0), i(0)
{
    // nothing to do
}

SFT::Value::Value(const float r_in, const float i_in) : r(r_in), i(i_in)
{
    // nothing to do
}

SFT::SFT(const vector<Sample> & samples) : _samples(samples)
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

SFT::Value SFT::Evaluate(const Coordinate & w) const
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

    return answer;
}
