/*
  ConflictChecker.hpp

  the base class for all Poisson disk conflict checkers
  
  Li-Yi Wei
  10/17/2010

*/

#ifndef _CONFLICT_CHECKER_HPP
#define _CONFLICT_CHECKER_HPP

#include "MetricField.hpp"
#include "SampleHolder.hpp"

class ConflictChecker
{
public:
    typedef enum {MEAN, MAX} Metric;
    typedef enum {CHECK_ERROR, CHECK_PASS, CHECK_FAIL} Status;

public:
    ConflictChecker(void);
    ConflictChecker(const Metric metric);
    virtual ~ConflictChecker(void) = 0;

    Metric ConflictMetric(void) const;

    // return true if the sample is in conflict with any sample in the set
    virtual Status Check(const Sample & query, const SampleHolder & sample_set) const;

protected:
    const Metric _metric; 
};

#endif
