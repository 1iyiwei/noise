/*
  RandomLFT.cpp

  Li-Yi Wei
  08/22/2008

*/

#include <iostream>
using namespace std;

#include "RandomLFT.hpp"
#include "Random.hpp"

#include <math.h>

RandomLFT::RandomLFT(const vector<Sample> & samples, const float percentage_input) : PFT(samples)
{
    // pre-compute selected pairs
    vector<int> all_pairs(_samples.size()*_samples.size());
    
    for(unsigned int i = 0; i < all_pairs.size(); i++)
    {
        all_pairs[i] = i;
    }

    float percentage = percentage_input;

    if(percentage > 1.0) percentage = 1.0;
    if(percentage < 0.0) percentage = 0.0;

    const int total_pair = floor(percentage*all_pairs.size());

    for(int k = 0; (k < total_pair) && (all_pairs.size() > 0); k++)
    {
        const int selection = static_cast<int>(floor(Random::UniformRandom()*all_pairs.size()))%all_pairs.size();

        const int current_pair = all_pairs[selection];

        all_pairs[selection] = all_pairs[all_pairs.size()-1];
        all_pairs.pop_back();

        _selected_pairs.push_back(current_pair);
    }
}

RandomLFT::~RandomLFT(void)
{
    // nothing to do
}

float RandomLFT::Evaluate(const Coordinate & w) const
{
    float answer = 0;
    for(int k = 0; k < _selected_pairs.size(); k++)
    {
        {
            const int current_pair = _selected_pairs[k];

            const int i = current_pair/_samples.size();
            const int j = current_pair%_samples.size();

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
    }

    // normalization
    if(_selected_pairs.size() > 0)
    {
        answer *= (_samples.size()*_samples.size())/_selected_pairs.size();
    }

    return (answer);
}
