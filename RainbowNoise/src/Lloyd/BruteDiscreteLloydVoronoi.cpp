/*
  BruteDiscreteLloydVoronoi.cpp

  Li-Yi Wei
  07/16/2009

*/

#include "BruteDiscreteLloydVoronoi.hpp"
#include "Exception.hpp"

BruteDiscreteLloydVoronoi::BruteDiscreteLloydVoronoi(const Domain & domain) : DiscreteLloydVoronoi(domain)
{
    // nothing else to do
}

BruteDiscreteLloydVoronoi::~BruteDiscreteLloydVoronoi(void)
{
    // nothing else to do
}

int BruteDiscreteLloydVoronoi::Get(const vector<LloydSite> & sites, vector<LloydSample> & points) const
{
    int lord = -1;
    float dist = 0;
    for(unsigned int i = 0; i < points.size(); i++)
    {
        LloydSample & point = points[i];

        point.index.Clear();

        for(unsigned int j = 0; j < sites.size(); j++)
        {
            const LloydSite & site = sites[j];

            if(! _domain.Nearest(point.sample, site.sample, _nearest_site.sample))
            {
                throw Exception("BruteDiscreteLloydVoronoi::Get(): cannot find nearest site");
                return 0;
            }

            const float dist_new = Distance2(_nearest_site.sample, point.sample);

            if(! point.index.Get(site.sample.id, lord, dist))
            {
                throw Exception("BruteDiscreteLloydVoronoi::Get(): cannot get point info");
                return 0;
            }

            if((lord < 0) || (dist > dist_new))
            {
                if(! point.index.Set(site.sample.id, j, dist_new))
                {
                    throw Exception("BruteDiscreteLloydVoronoi::Get(): cannot set point info");
                    return 0;
                }
            }
        }
    }

    // done
    return 1;
}
