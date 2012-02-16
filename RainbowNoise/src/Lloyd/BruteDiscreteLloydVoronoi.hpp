/*
  BruteDiscreteLloydVoronoi.hpp

  brute force computation for discrete Lloyd Voronoi

  Li-Yi Wei
  07/16/2009

*/

#ifndef _BRUTE_DISCRETE_LLOYD_VORONOI_HPP
#define _BRUTE_DISCRETE_LLOYD_VORONOI_HPP

#include "DiscreteLloydVoronoi.hpp"

class BruteDiscreteLloydVoronoi : public DiscreteLloydVoronoi
{
public:
    BruteDiscreteLloydVoronoi(const Domain & domain);
    virtual ~BruteDiscreteLloydVoronoi(void);

    int Get(const vector<LloydSite> & sites, vector<LloydSample> & points) const;

protected:
    mutable LloydSample _nearest_site;
};

#endif
