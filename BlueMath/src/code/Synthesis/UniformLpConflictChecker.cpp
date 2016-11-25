/*
  UniformLpConflictChecker.cpp

  Li-Yi Wei
  04/23/2011

*/

#include <math.h>

#include "UniformLpConflictChecker.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "Exception.hpp"

UniformLpConflictChecker::UniformLpConflictChecker(const float p, const float min_distance): _p(p), _min_distance(min_distance)
{
    // nothing else to do
}

UniformLpConflictChecker::~UniformLpConflictChecker(void)
{
    // nothing else to do
}

UniformLpConflictChecker::Status UniformLpConflictChecker::Check(const Sample & query, const SampleHolder & sample_set) const
{
    const Domain & domain = sample_set.GetDomain();

    vector<const Sample *> neighbors;

    if(!sample_set.GetNeighbors(query, _min_distance*sqrt(domain.Dimension()*1.0), neighbors))
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
            throw Exception("UniformLpConflictChecker::Check(): query.coordinate.Dimension() != neighbor.coordinate.Dimension()");
        }

        if(!domain.Nearest(query, neighbor, corrected)) return CHECK_ERROR;

        if(query.coordinate.Dimension() != corrected.coordinate.Dimension())
        {
            throw Exception("UniformLpConflictChecker::Check(): query.coordinate.Dimension() != corrected.coordinate.Dimension()");
        }

        const float distancep = (Math::IsINF(_p) ? Utility::DistanceINF(query.coordinate, corrected.coordinate) : pow(Utility::Distancep(_p, query.coordinate, corrected.coordinate)*1.0, 1.0/_p));

        if(distancep < _min_distance)
        {
            return CHECK_FAIL;
        }
    }

    return CHECK_PASS;
}
