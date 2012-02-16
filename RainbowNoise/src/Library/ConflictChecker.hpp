/*
  ConflictChecker.hpp

  the base class of all conflict checkers

  Li-Yi Wei
  04/25/2008

*/

#ifndef _CONFLICT_CHECKER_HPP
#define _CONFLICT_CHECKER_HPP

#include "Sample.hpp"

class ConflictChecker
{
public:
    typedef enum {MEAN, MAX} Metric;

public:
    ConflictChecker(void);
    ConflictChecker(const Metric metric);
    virtual ~ConflictChecker(void) = 0;

    // return MAX by default
    Metric ConflictMetric(void) const;

    typedef enum {CHECK_ERROR, CHECK_PASS, CHECK_FAIL} Status;

    // return true if the 2 samples are not in conflict
    virtual Status Check(const Sample & s1, const Sample & s2) const;

    // maximum possible distance between 2 samples to be in conflict
    virtual float MaxDistance(void) const;

protected:
    // return negative if error (e.g. samples in different dimensions)
    static float Distance(const Sample & s1, const Sample & s2);

protected:
    const Metric _metric; 
    mutable vector<int> _query; // used in Check to avoid repeated memoc
};

#endif
