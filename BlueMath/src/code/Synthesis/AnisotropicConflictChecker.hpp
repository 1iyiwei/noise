/*
  AnisotropicConflictChecker.hpp

  for anisotropic sampling

  Li-Yi Wei
  10/20/2010

*/

#ifndef _ANISOTROPIC_CONFLICT_CHECKER
#define _ANISOTROPIC_CONFLICT_CHECKER

#include "ConflictChecker.hpp"
#include "JacobianField.hpp"

class AnisotropicConflictChecker : public ConflictChecker
{
public:
    AnisotropicConflictChecker(const JacobianField & jacobian_field);
    virtual ~AnisotropicConflictChecker(void);

    Status Check(const Sample & query, const SampleHolder & sample_set) const;

protected:
    static float Norm2(const Matrix & jacobian, const Coordinate & diff);

protected:
    const JacobianField & _jacobian_field;
};

#endif
