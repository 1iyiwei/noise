/*
  SampledJacobianField.cpp

  Li-Yi Wei
  10/21/2010

*/

#include "SampledJacobianField.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

SampledJacobianField::SampledJacobianField(const Domain & domain, const Array<Matrix> & inverse_jacobians, const float cell_size, const float r_value) : DiscreteJacobianField(domain, cell_size)
{
    CommonConstruct(inverse_jacobians, r_value);
}

SampledJacobianField::~SampledJacobianField(void)
{
    // nothing else to do
}
