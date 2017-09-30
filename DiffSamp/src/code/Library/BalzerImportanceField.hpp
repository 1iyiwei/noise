/*
  BalzerImportanceField.hpp

  the analytical importance field in the teaser figure of [Balzer et al. SIGGRAPH 2009]

  Li-Yi Wei
  10/11/2010

*/

#ifndef _BALZER_IMPORTANCE_FIELD_HPP
#define _BALZER_IMPORTANCE_FIELD_HPP

#include "ImportanceField.hpp"

class BalzerImportanceField : public ImportanceField
{
public:

    // exp(exp_value*query^2 + sin_weight*(prod_i sin^2(pi*sin_freq*query_i)))
    // in their paper, exp_value = -20, sin_weight = 0.2, sin_freq = 1
    BalzerImportanceField(const int dimension, const float exp_value, const float sin_weight, const float sin_freq);
    ~BalzerImportanceField(void);

    int Dimension(void) const;

    float Query(const Sample & sample) const;

protected:
    const int _dimension;
    const float _exp_value, _sin_weight, _sin_freq;
};

#endif
