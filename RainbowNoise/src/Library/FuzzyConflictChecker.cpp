/*
  FuzzyConflictChecker.cpp

  Li-Yi Wei
  08/18/2008

*/

#include "FuzzyConflictChecker.hpp"
#include "Random.hpp"
#include "Exception.hpp"

#include <math.h>

FuzzyConflictChecker::Profiler::~Profiler(void)
{
    // nothing to do
}

FuzzyConflictChecker::Status FuzzyConflictChecker::Profiler::Check(const float min_distance, const float query_distance) const
{
    // shouldn't be called
    throw Exception("FuzzyConflictChecker::Profiler::Check() shouldn't be called!");

    return CHECK_ERROR;
}

FuzzyConflictChecker::FuzzyConflictChecker(const Array<float> & r_matrix, const Profiler & profiler) : _r_matrix(r_matrix), _profiler(profiler), _max_r_value(0)
{
    // make sure the matrix is 2D
    if(r_matrix.Dimension() != 2)
    {
        throw Exception("r_matrix not 2D");
    }

    // find maximum value
    {
        vector<float> values;
        r_matrix.Get(values);

        for(unsigned int i = 0; i < values.size(); i++)
        {
            if(values[i] > _max_r_value)
            {
                _max_r_value = values[i];
            }
        }
    }
}

FuzzyConflictChecker::Status FuzzyConflictChecker::Check(const Sample & s1, const Sample & s2) const
{
    vector<int> query(2, 0);
    query[0] = s1.id; query[1] = s2.id;

    float required_distance;

    if(! _r_matrix.Get(query, required_distance)) 
    {
        return CHECK_ERROR;
    }

    const float distance = Distance(s1, s2);

    return _profiler.Check(required_distance, distance);
}

float FuzzyConflictChecker::MaxDistance(void) const
{
    return _max_r_value;
}

FuzzyConflictChecker::Status LinearProfiler::Check(const float min_distance, const float query_distance) const
{
    if(Random::UniformRandom() < query_distance/min_distance)
    {
        return FuzzyConflictChecker::CHECK_PASS;
    }
    else
    {
        return FuzzyConflictChecker::CHECK_FAIL;
    }
}

MonomialProfiler::MonomialProfiler(const int degree) : _degree(degree)
{
    // nothing to do
}

FuzzyConflictChecker::Status MonomialProfiler::Check(const float min_distance, const float query_distance) const
{
    if(Random::UniformRandom() < pow(query_distance/min_distance*1.0, _degree*1.0))
    {
        return FuzzyConflictChecker::CHECK_PASS;
    }
    else
    {
        return FuzzyConflictChecker::CHECK_FAIL;
    }
}
