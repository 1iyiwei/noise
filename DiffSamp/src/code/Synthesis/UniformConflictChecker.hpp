/*
  UniformConflictChecker.hpp

  for uniform sampling

  Li-Yi Wei
  10/17/2010

*/

#ifndef _UNIFORM_CONFLICT_CHECKER
#define _UNIFORM_CONFLICT_CHECKER

#include "ConflictChecker.hpp"

class UniformConflictChecker : public ConflictChecker
{
public:
    UniformConflictChecker(const float min_distance);
    virtual ~UniformConflictChecker(void);

    Status Check(const Sample & query, const SampleHolder & sample_set) const;

protected:
    const float _min_distance;
};

#endif
