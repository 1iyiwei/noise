/*
  IsotropicDeformer.hpp

  Li-Yi Wei
  08/26/2008

*/

#ifndef _ISOTROPIC_DEFORMER_HPP
#define _ISOTROPIC_DEFORMER_HPP

#include "Deformer.hpp"
#include "DistanceField.hpp"

class IsotropicDeformer : public Deformer
{
public:
    IsotropicDeformer(const DistanceField & field, const float global_mean);
    ~IsotropicDeformer(void);

    float Query(const Sample & s1, const Sample & s2) const;

protected:
    const DistanceField & _distance_field;
    const float _global_mean;
};

#endif

