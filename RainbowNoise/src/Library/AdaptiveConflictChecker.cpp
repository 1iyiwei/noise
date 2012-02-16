/*
  AdaptiveConflictChecker.cpp

  Li-Yi Wei
  09/11/2008

*/

#include <algorithm>
using namespace std;

#include "AdaptiveConflictChecker.hpp"

AdaptiveConflictChecker::AdaptiveConflictChecker(const RMatrixField & r_matrix_field, const Metric metric) : ConflictChecker(metric), _r_matrix_field(r_matrix_field)
{
    // nothing else to do
}

AdaptiveConflictChecker::Status AdaptiveConflictChecker::Check(const Sample & s1, const Sample & s2) const
{
    const Array<float> & r_matrix_1 = _r_matrix_field.Query(s1);
    const Array<float> & r_matrix_2 = _r_matrix_field.Query(s2);

    // vector<int> query(2);
    vector<int> & query = _query;
    query[0] = s1.id; query[1] = s2.id;

    float required_distance_1 = 0;
    float required_distance_2 = 0;

    if((!r_matrix_1.Get(query, required_distance_1)) || (!r_matrix_2.Get(query, required_distance_2)))
    {
        return CHECK_ERROR;
    }

    const float distance = Distance(s1, s2);

    if(((_metric == MEAN) && (distance < 0.5*(required_distance_1 + required_distance_2))) || ((_metric == MAX) && (distance < max(required_distance_1, required_distance_2))))
    {
        return CHECK_FAIL;
    }
    else
    {
        return CHECK_PASS;
    }
}

float AdaptiveConflictChecker::MaxDistance(void) const
{
    return _r_matrix_field.MaxDistance();
}
