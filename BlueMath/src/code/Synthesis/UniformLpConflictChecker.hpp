/*
  UniformLpConflictChecker.hpp

  for uniform Lp sampling

  Li-Yi Wei
  04/23/2011

*/

#ifndef _UNIFORM_LP_CONFLICT_CHECKER
#define _UNIFORM_LP_CONFLICT_CHECKER

#include "ConflictChecker.hpp"

class UniformLpConflictChecker : public ConflictChecker
{
public:
    UniformLpConflictChecker(const float p, const float min_distance);
    virtual ~UniformLpConflictChecker(void);

    Status Check(const Sample & query, const SampleHolder & sample_set) const;

protected:
    const float _p;
    const float _min_distance;
};

#endif
