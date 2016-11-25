/*
  IsotropicConflictChecker.cpp

  Li-Yi Wei
  10/20/2010

*/

#include <algorithm>
using namespace std;

#include "IsotropicConflictChecker.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

IsotropicConflictChecker::IsotropicConflictChecker(const DistanceField & distance_field): _distance_field(distance_field)
{
    // nothing else to do
}

IsotropicConflictChecker::~IsotropicConflictChecker(void)
{
    // nothing else to do
}

IsotropicConflictChecker::Status IsotropicConflictChecker::Check(const Sample & query, const SampleHolder & sample_set) const
{
    const Domain & domain = sample_set.GetDomain();

    vector<const Sample *> neighbors;

    if(!sample_set.GetNeighbors(query, _distance_field.Max(), neighbors))
    {
        return CHECK_ERROR;
    }

    Sample corrected;

    for(unsigned int i = 0; i < neighbors.size(); i++)
    {
        if(!neighbors[i]) return CHECK_ERROR;

        const Sample & neighbor = *neighbors[i];

        if(query.coordinate.Dimension() != neighbor.coordinate.Dimension())
        {
            throw Exception("IsotropicConflictChecker::Check(): query.coordinate.Dimension() != neighbor.coordinate.Dimension()");
        }

        if(!domain.Nearest(query, neighbor, corrected)) return CHECK_ERROR;

        if(query.coordinate.Dimension() != corrected.coordinate.Dimension())
        {
            throw Exception("IsotropicConflictChecker::Check(): query.coordinate.Dimension() != corrected.coordinate.Dimension()");
        }

        const float distance2 = Utility::Distance2(query.coordinate, corrected.coordinate);
        const float min_distance = max(_distance_field.Query(query), _distance_field.Query(neighbor));

        if(distance2 < min_distance*min_distance)
        {
            return CHECK_FAIL;
        }
    }

    return CHECK_PASS;
}
