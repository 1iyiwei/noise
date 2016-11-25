/*
  ConflictChecker.cpp

  Li-Yi Wei
  10/17/2010

*/

#include "ConflictChecker.hpp"
#include "Exception.hpp"

ConflictChecker::ConflictChecker(void) : _metric(MAX)
{
    // nothing else to do
}

ConflictChecker::ConflictChecker(const Metric metric) : _metric(metric)
{
    // nothing else to do
}

ConflictChecker::~ConflictChecker(void)
{
    // nothing else to do
}

ConflictChecker::Metric ConflictChecker::ConflictMetric(void) const
{
    return _metric;
}

ConflictChecker::Status ConflictChecker::Check(const Sample & query, const SampleHolder & sample_set) const
{
    throw Exception("ConflictChecker::Check(): shouldn't be called!");

    return CHECK_ERROR;
}
