/*
  Domain.hpp

  the base class for all kinds of sample domains

  Li-Yi Wei
  06/18/2009

*/

#ifndef _DOMAIN_HPP
#define _DOMAIN_HPP

#include <vector>
using namespace std;

#include "Sample.hpp"

class Domain
{
public:
    virtual ~Domain(void) = 0;

    virtual int Dimension(void) const;

    // return 1 if the query is inside the domain, 0 else
    virtual int Inside(const Sample & query) const;

    // hmm, should I provide a bounding box and externalize random sample
    // or produce the random sample directly here?
    // answer: both, for flexibility
    // return 1 if success, 0 else
    virtual int BoundingBox(vector<float> & answer) const;
    virtual int UniformRandom(Sample & answer) const;

    // find the nearest sample with respect to anchor for query
    // in terms of different boundary conditions
    virtual int Nearest(const Sample & anchor, const Sample & query, Sample & answer) const;

    // bring in a query that might be outside the domain
    // return 1 if successful, 0 else
    virtual int Adopt(const Sample & query, Sample & answer) const;
};

#endif
