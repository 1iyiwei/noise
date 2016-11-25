/*
  Domain.hpp

  the base class for all kinds of sample domains

  Li-Yi Wei
  06/18/2009

  10/17/2010: remove random sample functionality as it should be externalized

*/

#ifndef _DOMAIN_HPP
#define _DOMAIN_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"

class Domain
{
public:
    // only these two are provided as other tricks
    // such as reflective and repeat ought to be achieved
    // via boundary_none + proper distance field content padding
    // and crop out the central region of the final samples
    typedef enum {BOUNDARY_NONE, BOUNDARY_TOROIDAL} BoundaryCondition;

public:
    virtual ~Domain(void) = 0;

    virtual int Dimension(void) const;

    // return 1 if the query is inside the domain, 0 else
    virtual int Inside(const Sample & query) const;

    // return 1 if success, 0 else
    virtual int BoundingBox(vector<float> & answer) const;

    virtual BoundaryCondition Boundary(void) const;

    // find the nearest sample with respect to anchor for query
    // in terms of different boundary conditions
    virtual int Nearest(const Sample & anchor, const Sample & query, Sample & answer) const;

    // bring in a query that might be outside the domain
    // return 1 if successful, 0 else
    virtual int Adopt(const Sample & query, Sample & answer) const;
};

#endif
