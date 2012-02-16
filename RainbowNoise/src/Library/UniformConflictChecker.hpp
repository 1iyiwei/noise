/*
  UniformConflictChecker.hpp

  ignore the per sample r values
  and instead use a uniform r per (group) id

  Li-Yi Wei
  04/26/2008

*/

#ifndef _UNIFORM_CONFLICT_CHECKER_HPP
#define _UNIFORM_CONFLICT_CHECKER_HPP

#include "ConflictChecker.hpp"
#include "Array.hpp"

class UniformConflictChecker : public ConflictChecker
{
public:
    // the minimum distance matrix
    UniformConflictChecker(const Array<float> & r_matrix);

    Status Check(const Sample & s1, const Sample & s2) const;

    float MaxDistance(void) const;

protected:
    const Array<float> _r_matrix;
    float _max_r_value;
};

#endif
