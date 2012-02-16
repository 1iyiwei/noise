/*
  DeformableLFT.hpp

  the basic (non-accelerated) version to compute deformable fourier transform
  I need to orthogonalize the deformation term and the acceleration methods
  And I achieved this via Defomer and DeformableLFT class

  Li-Yi Wei
  08/26/2008
  
*/

#ifndef _DEFORMABLE_LFT_HPP
#define _DEFORMABLE_LFT_HPP

#include <vector>
using namespace std;

#include "PFT.hpp"
#include "Deformer.hpp"

class DeformableLFT : public PFT
{
public:
    DeformableLFT(const vector<float> & domain_size, const vector<Sample> & samples, const Deformer & deformer);
    DeformableLFT(const vector<float> & domain_size, const vector<Sample> & samples, const Deformer & deformer, const float distance_lower_bound, const float distance_upper_bound);
    ~DeformableLFT(void);

    float Evaluate(const Coordinate & w) const;

// protected: // debug
    static float Diff(const float domain_size, const float s1, const float s2);

protected:
    const vector<float> _domain_size;
    const Deformer & _deformer;
    const float _d2_lower_bound, _d2_upper_bound; // for banding
};
#endif
