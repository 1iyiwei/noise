/*
  BruteDiscreteBalzerVoronoi.hpp

  brute force computation for capacity constraint site assignment (Balzer)

  Li-Yi Wei
  07/24/2009

*/

#ifndef _BRUTE_DISCRETE_BALZER_VORONOI_HPP
#define _BRUTE_DISCRETE_BALZER_VORONOI_HPP

#include <string>
using namespace std;

#include "DiscreteLloydVoronoi.hpp"
#include "SequentialCounter.hpp"

class BruteDiscreteBalzerVoronoi : public DiscreteLloydVoronoi
{
public:
    BruteDiscreteBalzerVoronoi(const Domain & domain, const int num_class);
    virtual ~BruteDiscreteBalzerVoronoi(void);

    // assume all points already have associated sites for all classes
    // in order to preserve capacity constraint
    int Get(const vector<LloydSite> & sites, vector<LloydSample> & points) const;

    // init random assignment of points to sites
    static string Init(const Domain & domain, const vector<LloydSite> & sites, vector<LloydSample> & points);

protected:
    struct HeapElement
    {
        HeapElement(void) : index(-1), dist_old(0), dist_new(0), dist_diff(0) {};
        
        int operator<(const HeapElement & peer) const {return (dist_diff < peer.dist_diff);};

        int index; // which serf array entry

        float dist_old, dist_new;

        float dist_diff;
    };

    static int BuildHeap(const Domain & domain, const vector<int> & combo_class, const int s_hip, const int s_hop, const vector<LloydSite> & sites, const vector<LloydSample> & points, const vector< vector<int> > & serfs, vector<HeapElement> & heap, LloydSite & nearest_site);

protected:
    const int _num_class;

    mutable SequentialCounter _counter;
    mutable vector<int> _combo_class;

    // the index list of points belonging to each site
    mutable vector< vector<int> > _serfs;

    // the two heaps
    mutable vector<HeapElement> _hip, _hop;

    mutable LloydSite _nearest_site;
};

#endif
