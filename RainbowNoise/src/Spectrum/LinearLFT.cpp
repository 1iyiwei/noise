/*
  LinearLFT.cpp

  Li-Yi Wei
  08/22/2008

*/

#include "LinearLFT.hpp"
#include "Random.hpp"

#include <math.h>

LinearLFT::LinearLFT(const vector<Sample> & samples) : PFT(samples)
{
    // build visit order
    vector<int> order(samples.size());
    for(unsigned int i = 0; i < order.size(); i++)
    {
        order[i] = i;
    }

    _order.clear();
    while(order.size() > 0)
    {
        const int selection = static_cast<int>(floor(Random::UniformRandom()*order.size()))%order.size();

        _order.push_back(order[selection]);

        order[selection] = order[order.size()-1];
        order.pop_back();
    }
}

LinearLFT::LinearLFT(void)
{
    // nothing to do
}

LinearLFT::~LinearLFT(void)
{
    // nothing to do
}

float LinearLFT::Evaluate(const Coordinate & w) const
{    
    float answer = 0;
    for(unsigned int i = 0; i < _samples.size(); i++)
        for(unsigned int j = 0; j < 1; j++) // anchor point
        //for(unsigned j = (i+1)%_samples.size(); j < (i+1)%_samples.size()+1; j++) // circular chain, highly biased if _samples pre-sorted, or high noisy if _samples pre-randomized
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
        
            answer += cos(foo)*_samples[i].value;
        }

    return (answer);
}
