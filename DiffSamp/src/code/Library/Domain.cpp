/*
  Domain.cpp

  Li-Yi Wei
  06/18/2009

*/

#include "Domain.hpp"
#include "Exception.hpp"

Domain::~Domain(void)
{
    // nothing to do
}

int Domain::Dimension(void) const
{
    throw Exception("Domain::Dimension() should not be called");
    return 0;
}

int Domain::Inside(const Sample & query) const
{
    throw Exception("Domain::Inside() should not be called");
    return 0;
}

int Domain::BoundingBox(vector<float> & answer) const
{
    throw Exception("Domain::BoundingBox() should not be called");
    return 0;
}

Domain::BoundaryCondition Domain::Boundary(void) const
{
    return BOUNDARY_NONE;
}

int Domain::Nearest(const Sample & anchor, const Sample & query, Sample & answer) const
{
    answer = query;
    return 1;
}

int Domain::Adopt(const Sample & query, Sample & answer) const
{
    if(Inside(query))
    {
        answer = query;
        return 1;
    }
    else
    {
        return 0;
    }
}
