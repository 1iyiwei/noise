/*
  Random Local Fourier Transform

  pick a random subset of all distance pairs for evaluation

  Li-Yi Wei
  08/24/2008

*/

#ifndef _RANDOM_LFT_HPP
#define _RANDOM_LFT_HPP

#include <vector>
using namespace std;

#include "PFT.hpp"

class RandomLFT : public PFT
{
public:
    // the percentage indicates the relative amount of pairs that would be used
    RandomLFT(const vector<Sample> & samples, const float percentage);
    ~RandomLFT(void);

    float Evaluate(const Coordinate & w) const;

protected:
    vector<int> _selected_pairs;
};

#endif

