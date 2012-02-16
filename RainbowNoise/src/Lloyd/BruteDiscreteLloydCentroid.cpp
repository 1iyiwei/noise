/*
  BruteDiscreteLloydCentroid.cpp

  Li-Yi Wei
  07/16/2009

*/

#include <math.h>

#include "BruteDiscreteLloydCentroid.hpp"
#include "Math.hpp"
#include "Exception.hpp"

BruteDiscreteLloydCentroid::BruteDiscreteLloydCentroid(const Domain & domain, const ClassWeight & class_weight) : DiscreteLloydCentroid(domain, class_weight), _counter(class_weight.NumClass(), 0, 1)
{
     // nothing else to do
}

BruteDiscreteLloydCentroid::~BruteDiscreteLloydCentroid(void)
{
    // nothing else to do
}

float BruteDiscreteLloydCentroid::Get(const vector<LloydSample> & points, vector<LloydSite> & sites) const
{
    _old_sites = sites;

    if(_site_weights.size() != sites.size())
    {
        _site_weights = vector<float>(sites.size(), 0);
    }

    for(unsigned int i = 0; i < sites.size(); i++)
    {
        sites[i].sample.coordinate.Clear(0);
        _site_weights[i] = 0;
    }

    // centroid in continuous space
    _counter.Reset();
    _counter.Next(); // skip all zero case

    do
    {
        _counter.Get(_class_combo);
        
        const float class_combo_weight = _class_weight.Get(_class_combo);
        
        if(class_combo_weight <= 0) continue;

        for(unsigned int i = 0; i < points.size(); i++)
        {
            const LloydSample & point = points[i];
            const int lord = point.index.Get(_class_combo);

            if((lord < 0) || (lord >= sites.size()))
            {
                throw Exception("BruteDiscreteLloydCentroid::Get(): illegal site index");
                return -1;
            }

            const float combined_weight = class_combo_weight*point.density;
            _site_weights[lord] += combined_weight;
                
            const LloydSite & old_site = _old_sites[lord];
            LloydSite & site = sites[lord];

            if(! _domain.Nearest(old_site.sample, point.sample, _nearest_point))
            {
                throw Exception("BruteDiscreteLloydCentroid::Get(): cannot find nearest point");
                return -1;
            }
            
            for(int j = 0; j < site.sample.coordinate.Dimension(); j++)
            {
                site.sample.coordinate[j] += combined_weight*_nearest_point.coordinate[j];
            }
        }
    }
    while(_counter.Next());

    // normalization
    for(unsigned int i = 0; i < sites.size(); i++)
    {
        if(_site_weights[i] <= 0)
        {
            throw Exception("BruteDiscreteLloydCentroid::Get(): empty site");
            return -1;
        }

        for(int j = 0; j < sites[i].sample.coordinate.Dimension(); j++)
        {
            sites[i].sample.coordinate[j] /= _site_weights[i];
        }
    }

    // snapping to existing set of points
    if(_snap)
    {
        if(!Snap(points, sites))
        {
            throw Exception("BruteDiscreteLloydCentroid::Get(): error in snapping sites");
            return -1;
        }
    }
    else
    {
        if(!Adopt(sites))
        {
            throw Exception("BruteDiscreteLloydCentroid::Get(): error in adopting sites");
            return -1;
        }
    }

    // max movement of sites
    float max_move = 0;
 
    if(sites.size() != _old_sites.size())
    {
        throw Exception("BruteDiscreteLloydCentroid::Get(): new sites not matching old sites");
    }

    for(unsigned int i = 0; i < sites.size(); i++)
    {
        LloydSite & site = sites[i];
        const LloydSite & old_site = _old_sites[i];

        if(site.fixed)
        {
            site = old_site;
            continue;
        }

        if(! _domain.Nearest(site.sample, old_site.sample, _nearest_point))
        {
            throw Exception("BruteDiscreteLloydCentroid::Get(): cannot find nearest site");
            return -1;
        }

        const float move = Distance2(site.sample, _nearest_point);

        if(move > max_move)
        {
            max_move = move;
        }
    }
    
    if(max_move >= 0)
    {
        max_move = sqrt(max_move);
    }

    // done
    return max_move;
}

int BruteDiscreteLloydCentroid::Snap(const vector<LloydSample> & points, vector<LloydSite> & sites) const
{
    for(unsigned int i = 0; i < sites.size(); i++)
    {
        LloydSite & site = sites[i];

        float min_dist = Math::INF;
        int min_index = -1;
        for(unsigned int j = 0; j < points.size(); j++)
        {
            const LloydSample & point = points[j];

            if(! _domain.Nearest(site.sample, point.sample, _nearest_point))
            {
                throw Exception("BruteDiscreteLloydCentroid::Snap(): cannot find nearest point");
                return 0;
            }

            const float dist = Distance2(site.sample, _nearest_point);

            if(dist < min_dist)
            {
                min_dist = dist;
                min_index = j;
            }
        }

        if((min_index < 0) || (min_index >= points.size()))
        {
            throw Exception("BruteDiscreteLloydCentroid::Snap(): illegal min index");
            return 0;
        }

        site.sample.coordinate = points[min_index].sample.coordinate; // don't change site.sample.id
    }

    // done
    return 1;
}
