/*
  ConflictChecker.cpp

  Li-Yi Wei
  04/25/2008

*/

#include "ConflictChecker.hpp"
#include "Exception.hpp"

#include <math.h>

ConflictChecker::ConflictChecker(void) : _metric(MAX), _query(2)
{
    // nothing to do
}

ConflictChecker::ConflictChecker(const Metric metric) : _metric(metric), _query(2)
{
    // nothing to do
}

ConflictChecker::~ConflictChecker(void)
{
    // nothing to do
}

ConflictChecker::Metric ConflictChecker::ConflictMetric(void) const
{
    return _metric;
}

ConflictChecker::Status ConflictChecker::Check(const Sample & s1, const Sample & s2) const
{
    // should not be called
    throw Exception("ConflictChecker::Check() called");
    return CHECK_ERROR;
}

float ConflictChecker::MaxDistance(void) const
{
    // should not be called
    throw Exception("ConflictChecker::MaxDistance() called");
    return 0;
}

float ConflictChecker::Distance(const Sample & s1, const Sample & s2)
{
    if(s1.coordinate.Dimension() != s2.coordinate.Dimension())
    {
        return -1;
    }
    else
    {
        float answer = 0;
        for(int i = 0; i < s1.coordinate.Dimension(); i++)
        {
            const float foo = s1.coordinate[i] - s2.coordinate[i];
            answer += foo*foo;
        }

        return sqrt(answer);
    }
}
