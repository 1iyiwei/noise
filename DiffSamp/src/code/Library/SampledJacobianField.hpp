/*
  SampledJacobianField.hpp

  a Jacoian field derived from an underlying warp

  Li-Yi Wei
  10/21/2010

*/

#ifndef _SAMPLED_JACOBIAN_FIELD
#define _SAMPLED_JACOBIAN_FIELD

#include "DiscreteJacobianField.hpp"
#include "Matrix.hpp"

class SampledJacobianField : public DiscreteJacobianField
{
public:
    SampledJacobianField(const Domain & domain, const Array<Matrix> & inverse_jacobians, const float cell_size, const float r_value);
    virtual ~SampledJacobianField(void);
};

#endif
