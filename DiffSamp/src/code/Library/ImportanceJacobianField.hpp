/*
  ImportanceJacobianField.hpp

  a Jacoian field derived from a scalar importance field

  Li-Yi Wei
  10/29/2010

*/

#ifndef _IMPORTANCE_JACOBIAN_FIELD
#define _IMPORTANCE_JACOBIAN_FIELD

#include "DiscreteJacobianField.hpp"
#include "Matrix.hpp"

class ImportanceJacobianField : public DiscreteJacobianField
{
public:
    ImportanceJacobianField(const Domain & domain, const Array<float> & importance, const float max_importance, const float cell_size, const float r_value);
    virtual ~ImportanceJacobianField(void);

    static vector<float> DomainSize(const Array<float> & importance, const float cell_size);

    static int Convert(const Domain & domain, const Array<float> & importance, const float max_importance, const float cell_size, const float r_value, Array<Matrix> & inverse_jacobians);

};

#endif
