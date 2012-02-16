/*
  DeformableLFT.cpp

  Li-Yi Wei
  08/26/2008

*/

#include <math.h>

#include "DeformableLFT.hpp"
#include "Math.hpp"

DeformableLFT::DeformableLFT(const vector<float> & domain_size, const vector<Sample> & samples, const Deformer & deformer) : PFT(samples), _domain_size(domain_size), _deformer(deformer), _d2_lower_bound(-1.0), _d2_upper_bound(-1.0)
{
    // nothing else to do
}

DeformableLFT::DeformableLFT(const vector<float> & domain_size, const vector<Sample> & samples, const Deformer & deformer, const float distance_lower_bound, const float distance_upper_bound) : PFT(samples), _domain_size(domain_size), _deformer(deformer), _d2_lower_bound(distance_lower_bound*distance_lower_bound), _d2_upper_bound(distance_upper_bound*distance_upper_bound)
{
    // nothing else to do
}

DeformableLFT::~DeformableLFT(void)
{
    // nothing else to do
}

float DeformableLFT::Evaluate(const Coordinate & w) const
{
    float answer = 0;
    for(unsigned int i = 0; i < _samples.size(); i++)
        for(unsigned int j = 0; j < _samples.size(); j++)
        {
            if((w.Dimension() != _domain_size.size()) || (w.Dimension() != _samples[i].coordinate.Dimension()) || (w.Dimension() != _samples[j].coordinate.Dimension()))
            {
                // error
                answer = 0;
                break;
            }

            float d2 = 0;
            float foo = 0;
            for(int k = 0; k < w.Dimension(); k++)
            {
                const float diff = Diff(_domain_size[k], _samples[i].coordinate[k], _samples[j].coordinate[k]);
                foo += w[k]*diff;
                
                if(_d2_upper_bound > 0)
                {
                    d2 += diff*diff;
                }
            }
        
            if((_d2_upper_bound <= 0) || ((_d2_lower_bound <= d2) && (d2 < _d2_upper_bound)))
            {
                const float deformation = _deformer.Query(_samples[i], _samples[j]);

                answer += cos(foo*deformation)*_samples[i].value*_samples[j].value;
            }
        }

    return (answer);
}

float DeformableLFT::Diff(const float domain_size, const float s1, const float s2)
{
    const float diff = s1-s2;

#if 0
    // geodesic/toroidal distance
    // not really helpful
    const float abs_diff = fabs(diff);
    const float ds_minus_abs_diff = domain_size - abs_diff;
    return (diff + (abs_diff > ds_minus_abs_diff)*(2*(diff <= 0)-1)*domain_size);
    // return (abs_diff < ds_minus_abs_diff ? diff : (1-(diff >= 0)*2)*ds_minus_abs_diff);
#else
    // ordinary distance
    return diff;
#endif
}
