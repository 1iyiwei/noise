/*
  IsotropicConflictChecker.hpp

  for isotropic/adaptive sampling

  Li-Yi Wei
  10/20/2010

*/

#ifndef _ADAPTIVE_CONFLICT_CHECKER
#define _ADAPTIVE_CONFLICT_CHECKER

#include "ConflictChecker.hpp"
#include "DistanceField.hpp"

class IsotropicConflictChecker : public ConflictChecker
{
public:
    IsotropicConflictChecker(const DistanceField & distance_field);
    virtual ~IsotropicConflictChecker(void);

    Status Check(const Sample & query, const SampleHolder & sample_set) const;

protected:
    const DistanceField & _distance_field;
};

#endif
