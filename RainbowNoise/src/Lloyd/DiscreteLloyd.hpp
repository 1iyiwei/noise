/*
  DiscreteLloyd.hpp

  Lloyd relaxation in the discrete sample space

  Li-Yi Wei
  07/15/2009

*/

#ifndef _DISCRETE_LLOYD_HPP
#define _DISCRETE_LLOYD_HPP

#include "Lloyd.hpp"
#include "LloydSite.hpp"
#include "LloydSample.hpp"
#include "DiscreteLloydVoronoi.hpp"
#include "DiscreteLloydCentroid.hpp"

class DiscreteLloyd : public Lloyd
{
public:
    DiscreteLloyd(const DiscreteLloydVoronoi & voronoi, const DiscreteLloydCentroid & centroid);
    virtual ~DiscreteLloyd(void);
    
    virtual int Set(const vector<LloydSample> & points);
    virtual int Get(vector<LloydSample> & points) const;

    virtual int Set(const vector<LloydSite> & sites);
    virtual int Get(vector<LloydSite> & sites) const;
 
    int Voronoi(void);
    float Centroid(void);
    
protected:
    const DiscreteLloydVoronoi & _voronoi;
    const DiscreteLloydCentroid & _centroid;

    vector<LloydSite> _init_sites;
    vector<LloydSample> _points;
};
#endif
