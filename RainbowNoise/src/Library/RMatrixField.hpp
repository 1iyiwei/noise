/*
  RMatrixField.hpp

  the base class for all spatially varying RMatrix classes

  Li-Yi Wei
  09/11/2008

*/

#ifndef _R_MATRIX_FIELD_HPP
#define _R_MATRIX_FIELD_HPP

#include "Sample.hpp"
#include "RMatrix.hpp"

class RMatrixField
{
public:
    RMatrixField(void);
    virtual ~RMatrixField(void) = 0;

    // the r_matrix at a specific sample location
    // the class information is encoded into the sample
    virtual const Array<float> & Query(const Sample & sample) const;

    // maximum possible distance between 2 samples to be in conflict
    virtual float MaxDistance(void) const;

protected:
    Array<float> _foo;
};

#endif
