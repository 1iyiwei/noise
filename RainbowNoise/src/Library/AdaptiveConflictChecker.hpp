/*
  AdaptiveConflictChecker.hpp

  Li-Yi Wei
  09/11/2008

*/

#ifndef _ADAPTIVE_CONFLICT_CHECKER_HPP
#define _ADAPTIVE_CONFLICT_CHECKER_HPP

#include "ConflictChecker.hpp"
#include "RMatrixField.hpp"

class AdaptiveConflictChecker : public ConflictChecker
{
public:
    // the minimum distance matrix
    AdaptiveConflictChecker(const RMatrixField & r_matrix_field, const Metric metric);

    Status Check(const Sample & s1, const Sample & s2) const;

    float MaxDistance(void) const;

protected:
    const RMatrixField & _r_matrix_field;
};

#endif
