/*
  DiscreteLloydCentroid.hpp

  the base class for all centroid computation for discrete Lloyd relaxation

  Li-Yi Wei
  07/16/2009

*/

#ifndef _DISCRETE_LLOYD_CENTROID_HPP
#define _DISCRETE_LLOYD_CENTROID_HPP

#include "Domain.hpp"
#include "ClassWeight.hpp"
#include "LloydSite.hpp"
#include "LloydSample.hpp"

class DiscreteLloydCentroid
{
public:
    DiscreteLloydCentroid(const Domain & domain, const ClassWeight & class_weight);
    virtual ~DiscreteLloydCentroid(void) = 0;

    int NumClass(void) const;

    virtual float Get(const vector<LloydSample> & points, vector<LloydSite> & sites) const;

    // snap each site to the nearest point
    virtual int Snap(const vector<LloydSample> & points, vector<LloydSite> & sites) const;

    void Snap(const bool option);
    bool Snap(void) const;

    // adopt all sites into the domain
    int Adopt(vector<LloydSite> & sites) const;

protected:
    const Domain & _domain;
    const ClassWeight & _class_weight;
    bool _snap;
};

#endif
