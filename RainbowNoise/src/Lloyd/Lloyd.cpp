/*
  Lloyd.cpp

  Li-Yi Wei
  07/15/2009

*/

#include "Lloyd.hpp"
#include "Exception.hpp"

Lloyd::Lloyd(void)
{
    // nothing else to do
}

Lloyd::~Lloyd(void)
{
    // nothing else to do
}

int Lloyd::Set(const vector<LloydSite> & sites)
{
    _sites = sites;
    return 1;
}

int Lloyd::Get(vector<LloydSite> & sites) const
{
    sites = _sites;
    return 1;
}
 
int Lloyd::Voronoi(void)
{
    throw Exception("Lloyd::Voronoi(): should not be called");
    return 0;
}

float Lloyd::Centroid(void)
{
    throw Exception("Lloyd::Centroid(): should not be called");
    return -1;
}
