/*
  AnisotropicConflictChecker.cpp

  Li-Yi Wei
  10/17/2010

*/

#include "AnisotropicConflictChecker.hpp"
#include "Matrix.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

AnisotropicConflictChecker::AnisotropicConflictChecker(const JacobianField & jacobian_field): _jacobian_field(jacobian_field)
{
    // nothing else to do
}

AnisotropicConflictChecker::~AnisotropicConflictChecker(void)
{
    // nothing else to do
}

AnisotropicConflictChecker::Status AnisotropicConflictChecker::Check(const Sample & query, const SampleHolder & sample_set) const
{
    const Domain & domain = sample_set.GetDomain();

    vector<const Sample *> neighbors;

    if(!sample_set.GetNeighbors(query, _jacobian_field.ForwardMaxEigenValue(), neighbors))
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
            throw Exception("AnisotropicConflictChecker::Check(): query.coordinate.Dimension() != neighbor.coordinate.Dimension()");
        }

        if(!domain.Nearest(query, neighbor, corrected)) return CHECK_ERROR;

        if(query.coordinate.Dimension() != corrected.coordinate.Dimension())
        {
            throw Exception("AnisotropicConflictChecker::Check(): query.coordinate.Dimension() != corrected.coordinate.Dimension()");
        }

        Matrix jacobian_query, jacobian_neighbor;
        
        if(!_jacobian_field.Inverse(query, jacobian_query) || !_jacobian_field.Inverse(neighbor, jacobian_neighbor))
        {
            return CHECK_ERROR;
        }

        Coordinate diff = Utility::Diff(query.coordinate, corrected.coordinate);

        const float query_norm2 = Norm2(jacobian_query, diff);
        const float neighbor_norm2 = Norm2(jacobian_neighbor, diff);

        if((query_norm2 < 1.0) || (neighbor_norm2 < 1.0))
        {
            return CHECK_FAIL;
        }
    }

    return CHECK_PASS;
}

float AnisotropicConflictChecker::Norm2(const Matrix & jacobian, const Coordinate & diff)
{
    Coordinate transformed;
    if(! jacobian.Multiply(diff, transformed)) throw Exception("AnisotropicConflictChecker::Norm2(): cannot multiply");

    return Utility::Norm2(transformed);
}
