/*
  SampledRMatrixField.hpp

  Li-Yi Wei
  09/11/2008

*/

#ifndef _SAMPLED_R_MATRIX_FIELD_HPP
#define _SAMPLED_R_MATRIX_FIELD_HPP

#include "RMatrixField.hpp"
#include "SampledDistanceField.hpp"

class SampledRMatrixField : public RMatrixField
{
public:
    SampledRMatrixField(const SampledDistanceField & sampled_distance_field);
    ~SampledRMatrixField(void);

    const Array<float> & Query(const Sample & sample) const;
    
    float MaxDistance(void) const;

protected:
    const vector<float> _domain_size;
    // a big pre-computed array with dimension = num_classes*2 + dimension of sample space
    Array< Array<float> > _data;
    float _max_distance;

    mutable vector<int> _index; // used in Query to avoid repeated melloc
    mutable Array<float> _foo; // used in query to avoid repeated melloc
};

#endif
