/*
  FuzzyConflictChecker.hpp

  similar to UniformConflictChecker
  but use a probabilistic method to accept/reject samples

  Li-Yi Wei
  08/18/2008

*/

#ifndef _FUZZY_CONFLICT_CHECKER_HPP
#define _FUZZY_CONFLICT_CHECKER_HPP

#include "ConflictChecker.hpp"
#include "Array.hpp"

class FuzzyConflictChecker : public ConflictChecker
{
public:
    class Profiler
    {
    public:
        virtual ~Profiler(void) = 0;

        // this function will behave probabilistically
        // with different profiles substantiated by sub-classes
        virtual Status Check(const float min_distance, const float query_distance) const;
    };

public:
    // the minimum distance matrix
    FuzzyConflictChecker(const Array<float> & r_matrix, const Profiler & profiler);

    Status Check(const Sample & s1, const Sample & s2) const;

    float MaxDistance(void) const;

protected:
    const Array<float> _r_matrix;
    const Profiler & _profiler;
    float _max_r_value;
};

class LinearProfiler : public FuzzyConflictChecker::Profiler
{
public:
    FuzzyConflictChecker::Status Check(const float min_distance, const float query_distance) const;

};

class MonomialProfiler : public FuzzyConflictChecker::Profiler
{
public:
    MonomialProfiler(const int degree);

    FuzzyConflictChecker::Status Check(const float min_distance, const float query_distance) const;

protected:
    const int _degree;
};

#endif
