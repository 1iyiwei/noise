/*
  AdaptiveAFT.cpp

  Li-Yi Wei
  08/30/2010

*/

#include <math.h>

#include "AdaptiveAFT.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

AdaptiveAFT::AdaptiveAFT(const int num_samples, const float normalization_const, const vector<float> & domain_size, const Array<float> & importance) : AFT(num_samples, normalization_const), _domain_size(domain_size), _importance(importance), _cell_size(domain_size), _counter(BuildCounter(importance))
{
    if(domain_size.size() != importance.Dimension())
    {
        throw Exception("AdaptiveAFT::AdaptiveAFT(): dimension mismatch");
    }

    for(unsigned int i = 0; i < _cell_size.size(); i++)
    {
        _cell_size[i] = domain_size[i]/importance.Size(i);
    }
}

AdaptiveAFT::~AdaptiveAFT(void)
{
    // nothing else to do
}

float AdaptiveAFT::Evaluate(const Coordinate & w) const
{
    if(w.Dimension() != _importance.Dimension()) throw Exception("AdaptiveAFT::Evaluate(): dimension mismatch");

    float cos_sum = 0;
    float sin_sum = 0;

    vector<int> index(w.Dimension());
    vector<float> box_min(w.Dimension());
    vector<float> box_max(w.Dimension());

    vector<float> wreq;
    Utility::Convert(w, wreq);

    float importance_value = 0;

    _counter.Reset();
    
    do
    {
        _counter.Get(index);

        if(index.size() != w.Dimension()) throw Exception("AdaptiveAFT::Evaluate(): index.size() != w.Dimension()");

        for(unsigned int i = 0; i < box_min.size(); i++)
        {
            box_min[i] = index[i]*_cell_size[i];
        }

        for(unsigned int i = 0; i < box_max.size(); i++)
        {
            box_max[i] = box_min[i] + _cell_size[i];
        }

        if(! _importance.Get(index, importance_value))  throw Exception("AdaptiveAFT::Evaluate(): cannot get importance value");

        cos_sum += importance_value * Icossin(wreq, box_min, box_max, COS);
        sin_sum += importance_value * Icossin(wreq, box_min, box_max, SIN);
    }
    while(_counter.Next());

    float value = 1 + (_num_samples - 1)*(cos_sum*cos_sum + sin_sum*sin_sum);

    value *= _num_samples*_normalization_const*_normalization_const;

    return value;
}

float AdaptiveAFT::Icossin(const vector<float> & w, const vector<float> & box_min, const vector<float> & box_max, const Mode mode)
{
    if((w.size() != box_min.size()) || (w.size() != box_max.size()))
    {
        throw Exception("AdaptiveAFT::Icossin(): dimension mismatch");
    }

    if(w.size() <= 0) throw Exception("AdaptiveAFT::Icossin(): non-positive dimension");

    if(w.size() == 1)
    {
        if(w[0] == 0)
        {
            if(mode == COS)
            {
                return (box_max[0] - box_min[0]);
            }
            else // Mode == SIN
            {
                return 0;
            }
        }
        else // w[0] != 0
        {
            if(mode == COS)
            {
                return (sin(w[0]*box_max[0]) - sin(w[0]*box_min[0]))/w[0];
            }
            else // Mode == SIN
            {
                return (-cos(w[0]*box_max[0]) + cos(w[0]*box_min[0]))/w[0];
            }
        }
    }
    else // w.size() > 1
    {
        vector<float> w_head(1, w[w.size()-1]);
        vector<float> w_tail(w); w_tail.pop_back();
        vector<float> head_min(1, box_min[box_min.size()-1]);
        vector<float> head_max(1, box_max[box_max.size()-1]);
        vector<float> tail_min(box_min); tail_min.pop_back();
        vector<float> tail_max(box_max); tail_max.pop_back();

        if(mode == COS)
        {
            return Icossin(w_head, head_min, head_max, COS)*Icossin(w_tail, tail_min, tail_max, COS) - Icossin(w_head, head_min, head_max, SIN)*Icossin(w_tail, tail_min, tail_max, SIN);
        }
        else // Mode == SIN
        {
            return Icossin(w_head, head_min, head_max, SIN)*Icossin(w_tail, tail_min, tail_max, COS) + Icossin(w_head, head_min, head_max, COS)*Icossin(w_tail, tail_min, tail_max, SIN);
        }
    }
}

SequentialCounter AdaptiveAFT::BuildCounter(const Array<float> & importance)
{
    vector<int> data_size;
    importance.Size(data_size);

    for(unsigned int i = 0; i < data_size.size(); i++)
    {
        data_size[i] -= 1;
    }

    SequentialCounter counter(importance.Dimension(), vector<int>(importance.Dimension(), 0), data_size);

    return counter;
}
