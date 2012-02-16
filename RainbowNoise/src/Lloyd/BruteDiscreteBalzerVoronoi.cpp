/*
  BruteDiscreteBalzerVoronoi.cpp

  Li-Yi Wei
  07/16/2009

*/

#include <algorithm>
using namespace std;

#include "BruteDiscreteBalzerVoronoi.hpp"
#include "Random.hpp"
#include "Utility.hpp"
#include "LloydTools.hpp"
#include "Exception.hpp"

BruteDiscreteBalzerVoronoi::BruteDiscreteBalzerVoronoi(const Domain & domain, const int num_class) : DiscreteLloydVoronoi(domain), _num_class(num_class), _counter(num_class, vector<int>(num_class, 0), vector<int>(num_class, 1))
{
    if(_num_class <= 0)
    {
        throw Exception("BruteDiscreteBalzerVoronoi::BruteDiscreteBalzerVoronoi(): num_class must > 0");
    }
}

BruteDiscreteBalzerVoronoi::~BruteDiscreteBalzerVoronoi(void)
{
    // nothing else to do
}

string BruteDiscreteBalzerVoronoi::Init(const Domain & domain, const vector<LloydSite> & sites, vector<LloydSample> & points)
{
    vector<int> num_sites;    
    if(! LloydTools::CountSamples(sites, num_sites))
    {
        return "error in counting samples";
    }

    const int num_class = num_sites.size();

    vector< vector<int> > class_sites(num_class);
    for(unsigned int i = 0; i < sites.size(); i++)
    {
        class_sites[sites[i].sample.id].push_back(i);
    }

    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i].index.Clear(num_class);
    }

    SequentialCounter counter(num_class, vector<int>(num_class, 0), vector<int>(num_class, 1));
    vector<int> combo_class;

    counter.Reset();
    counter.Next(); // skip the all 0 case

    do
    {
        counter.Get(combo_class);

        vector<int> combo_sites;

        for(unsigned int i = 0; i < combo_class.size(); i++)
        {
            if(combo_class[i])
            {
                // append class_sites[i] to combo_sites
                combo_sites.insert(combo_sites.end(), class_sites[i].begin(), class_sites[i].end());
            }
        }

        vector<int> lords(points.size());
        for(unsigned int i = 0; i < lords.size(); i++)
        {
            lords[i] = combo_sites[i%combo_sites.size()];
        }

        Randomize(lords);

        LloydSite nearest_site;

        for(unsigned int i = 0; i < points.size(); i++)
        {
            LloydSample & point = points[i];

            const int lord = lords[i];
            const LloydSite & site = sites[lord];

            if(! domain.Nearest(point.sample, site.sample, nearest_site.sample))
            {
                throw Exception("BruteDiscreteBalzerVoronoi::Init(): cannot find nearest site");
            }

            const float dist = Distance2(point.sample, nearest_site.sample);

            if(! point.index.Set(combo_class, lord, dist))
            {
                throw Exception("BruteDiscreteBalzerVoronoi::Init(): cannot set point index");
            }
        }
    }
    while(counter.Next());

    // done
    return "";
}

int BruteDiscreteBalzerVoronoi::Get(const vector<LloydSite> & sites, vector<LloydSample> & points) const
{
    const int num_site = sites.size();

    if(_serfs.size() != sites.size())
    {
        _serfs = vector< vector<int> >(sites.size());
    }

    int lord = -1;
    float dist = 0;

    _counter.Reset();
    _counter.Next();
    
    do
    {
        _counter.Get(_combo_class);

        {
            // build serfs
            for(unsigned int i = 0; i < _serfs.size(); i++)
            {
                _serfs[i].clear();
            }

            for(unsigned int i = 0; i < points.size(); i++)
            {
                if(! points[i].index.Get(_combo_class, lord, dist))
                {
                    throw Exception("BruteDiscreteBalzerVoronoi::Get(): cannot get index info");
                    return 0;
                }

                if((lord < 0) || ((static_cast<unsigned int>(lord) >= _serfs.size())))
                {
                    throw Exception("BruteDiscreteBalzerVoronoi::Get(): problematic lord");
                    return 0;
                }

                _serfs[lord].push_back(i);
            }
        }

        int stable = 0;
        do
        {
            stable = 1;

            // build heaps
            for(int s_hip = 0; s_hip < num_site; s_hip++)
            {
                for(int s_hop = s_hip+1; s_hop < num_site; s_hop++)
                {
                    if(! BuildHeap(_domain, _combo_class, s_hip, s_hop, sites, points, _serfs, _hip, _nearest_site))
                    {
                        throw Exception("BruteDiscreteBalzerVoronoi::Get(): error in building hip");
                    }

                    if(! BuildHeap(_domain, _combo_class, s_hop, s_hip, sites, points, _serfs, _hop, _nearest_site))
                    {
                        throw Exception("BruteDiscreteBalzerVoronoi::Get(): error in building hop");
                    }
          
                    // process heaps
                    while((_hip.size() > 0) && (_hop.size() > 0) && ((_hip[_hip.size()-1].dist_diff + _hop[_hop.size()-1].dist_diff) > 0))
                    {
                        // swap
                        const HeapElement & element_hip = _hip[_hip.size()-1];
                        const HeapElement & element_hop = _hop[_hop.size()-1];

                        LloydSample & point_hip = points[_serfs[s_hip][element_hip.index]];
                        LloydSample & point_hop = points[_serfs[s_hop][element_hop.index]];

                        if(! point_hip.index.Set(_combo_class, s_hop, element_hip.dist_new))
                        {
                            throw Exception("cannot update point hip");
                            return 0;
                        }
                        if(! point_hop.index.Set(_combo_class, s_hip, element_hop.dist_new))
                        {
                            throw Exception("cannot update point hop");
                            return 0;
                        }

                        swap(_serfs[s_hip][element_hip.index], _serfs[s_hop][element_hop.index]);

                        // pop heap
                        _hip.pop_back();
                        _hop.pop_back();

                        stable = 0;
                    }
                }
            }
        }
        while(!stable);
    }
    while(_counter.Next());

    // done
    return 1;
}

int BruteDiscreteBalzerVoronoi::BuildHeap(const Domain & domain, const vector<int> & combo_class, const int s_hip, const int s_hop, const vector<LloydSite> & sites, const vector<LloydSample> & points, const vector< vector<int> > & serfs, vector<HeapElement> & heap, LloydSite & nearest_site)
{
    const LloydSite & site_hip = sites[s_hip];
    const LloydSite & site_hop = sites[s_hop];

    heap.clear();

    int lord = -1;
    HeapElement element;
    for(unsigned int i = 0; i < serfs[s_hip].size(); i++)
    {
        const LloydSample & point = points[serfs[s_hip][i]];
        
        element.index = i;
        if(! point.index.Get(combo_class, lord, element.dist_old))
        {
            throw Exception("BruteDiscreteBalzerVoronoi::BuildHeap(): cannot get index info");
            return 0;
        }

        if(! domain.Nearest(point.sample, site_hop.sample, nearest_site.sample))
        {
            throw Exception("BruteDiscreteBalzerVoronoi::BuildHeap(): cannot find nearest site");
            return 0;
        }
                        
        element.dist_new = Distance2(point.sample, nearest_site.sample);
        element.dist_diff = element.dist_old - element.dist_new;

        heap.push_back(element);
    }

    sort(heap.begin(), heap.end());

    return 1;
}
