/*
  DiscreteLloydVoronoi.hpp

  the base class for all Voronoi computation for discrete Lloyd relaxation

  Li-Yi Wei
  07/16/2009

*/

#ifndef _DISCRETE_LLOYD_VORONOI_HPP
#define _DISCRETE_LLOYD_VORONOI_HPP

#include "Domain.hpp"
#include "LloydSite.hpp"
#include "LloydSample.hpp"

class DiscreteLloydVoronoi
{
public:
    DiscreteLloydVoronoi(const Domain & domain);
    virtual ~DiscreteLloydVoronoi(void) = 0;

    // the input points already contain proper info like sample locations
    // and this function only fills in the LloydSample.index info
    virtual int Get(const vector<LloydSite> & sites, vector<LloydSample> & points) const;
    
protected:
    const Domain & _domain;
};
#endif
