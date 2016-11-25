/*
  Adaptive Analytical Power Fourier Transform

  Li-Yi Wei
  08/30/2010

*/

#ifndef _ADAPTIVE_AFT_HPP
#define _ADAPTIVE_AFT_HPP

#include <vector>
using namespace std;

#include "AFT.hpp"
#include "Array.hpp"
#include "SequentialCounter.hpp"

class AdaptiveAFT : public AFT
{
public:
    AdaptiveAFT(const int num_samples, const float normalization_const, const vector<float> & domain_size, const Array<float> & importance);
    virtual ~AdaptiveAFT(void);

    virtual float Evaluate(const Coordinate & w) const;

protected:
    enum Mode {COS, SIN};

    static float Icossin(const vector<float> & w, const vector<float> & box_min, const vector<float> & box_max, const Mode mode);

    static SequentialCounter BuildCounter(const Array<float> & importance);

protected:
    const vector<float> _domain_size;
    const Array<float> _importance;
    vector<float> _cell_size;
    
    mutable SequentialCounter _counter;
};

#endif

