/*
  UniformConflictChecker.cpp

  Li-Yi Wei
  10/17/2010

*/

#include "UniformConflictChecker.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

UniformConflictChecker::UniformConflictChecker(const float min_distance): _min_distance(min_distance)
{
    // nothing else to do
}

UniformConflictChecker::~UniformConflictChecker(void)
{
    // nothing else to do
}

UniformConflictChecker::Status UniformConflictChecker::Check(const Sample & query, const SampleHolder & sample_set) const
{
    const Domain & domain = sample_set.GetDomain();

    vector<const Sample *> neighbors;

    if(!sample_set.GetNeighbors(query, _min_distance, neighbors))
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
            throw Exception("UniformConflictChecker::Check(): query.coordinate.Dimension() != neighbor.coordinate.Dimension()");
        }

        if(!domain.Nearest(query, neighbor, corrected)) return CHECK_ERROR;

        if(query.coordinate.Dimension() != corrected.coordinate.Dimension())
        {
            throw Exception("UniformConflictChecker::Check(): query.coordinate.Dimension() != corrected.coordinate.Dimension()");
        }

        const float distance2 = Utility::Distance2(query.coordinate, corrected.coordinate);

        if(distance2 < _min_distance*_min_distance)
        {
            return CHECK_FAIL;
        }
    }

    return CHECK_PASS;
}
