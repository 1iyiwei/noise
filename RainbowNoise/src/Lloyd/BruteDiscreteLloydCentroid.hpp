/*
  BruteDiscreteLloydCentroid.hpp

  brute force computation for discrete Lloyd centroid

  Li-Yi Wei
  07/16/2009

*/

#ifndef _BRUTE_DISCRETE_LLOYD_CENTROID_HPP
#define _BRUTE_DISCRETE_LLOYD_CENTROID_HPP

#include "DiscreteLloydCentroid.hpp"
#include "SequentialCounter.hpp"

class BruteDiscreteLloydCentroid : public DiscreteLloydCentroid
{
public:
    BruteDiscreteLloydCentroid(const Domain & domain, const ClassWeight & class_weight);
    virtual ~BruteDiscreteLloydCentroid(void);

    float Get(const vector<LloydSample> & points, vector<LloydSite> & sites) const;

    int Snap(const vector<LloydSample> & points, vector<LloydSite> & sites) const;

protected:
    mutable SequentialCounter _counter;
    mutable vector<int> _class_combo;
    mutable vector<float> _site_weights;
    mutable Sample _nearest_point;
    mutable vector<LloydSite> _old_sites;
};

#endif
