/*
  DiscreteLloyd.cpp

  Li-Yi Wei
  07/15/2009

*/

#include "DiscreteLloyd.hpp"
#include "Exception.hpp"

DiscreteLloyd::DiscreteLloyd(const DiscreteLloydVoronoi & voronoi, const DiscreteLloydCentroid & centroid) : _voronoi(voronoi), _centroid(centroid)
{
    // nothing else to do
}

DiscreteLloyd::~DiscreteLloyd(void)
{
    // nothing else to do
}
    
int DiscreteLloyd::Set(const vector<LloydSample> & points)
{
    _points = points;

    for(unsigned int i = 0; i < _points.size(); i++)
    {
        if(_points[i].index.NumClass() != _centroid.NumClass())
        {
            _points[i].index.Clear(_centroid.NumClass());
        }
    }

    return 1;
}

int DiscreteLloyd::Get(vector<LloydSample> & points) const
{
    points = _points;

    return 1;
}

int DiscreteLloyd::Set(const vector<LloydSite> & sites)
{
    if(! Lloyd::Set(sites)) return 0;

    _init_sites = sites;

    for(unsigned int i = 0; i < _points.size(); i++)
    {
        if(_points[i].index.NumClass() != _centroid.NumClass())
        {
            _points[i].index.Clear(_centroid.NumClass());
        }
    }
    
    return 1;
}

int DiscreteLloyd::Get(vector<LloydSite> & sites) const
{
    return Lloyd::Get(sites);
}
 
int DiscreteLloyd::Voronoi(void)
{
    return _voronoi.Get(_sites, _points);
}

float DiscreteLloyd::Centroid(void)
{
    const float value = _centroid.Get(_points, _sites);

#if 0 // assuming this is handled inside centroid class
    if(_sites.size() != _init_sites.size())
    {
        throw Exception("DiscreteLloyd::Centroid(): site count mismatch");
    }

    for(unsigned int i = 0; i < _init_sites.size(); i++)
    {
        if(_init_sites[i].fixed)
        {
            _sites[i] = _init_sites[i];
        }
    }
#endif

    return value;
}
