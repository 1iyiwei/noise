/*
  DiscreteLloydVoronoi.cpp

  Li-Yi Wei
  07/16/2009

*/

#include "DiscreteLloydVoronoi.hpp"
#include "Exception.hpp"

DiscreteLloydVoronoi::DiscreteLloydVoronoi(const Domain & domain) : _domain(domain)
{
    // nothing else to do
}

DiscreteLloydVoronoi::~DiscreteLloydVoronoi(void)
{
    // nothing else to do
}

int DiscreteLloydVoronoi::Get(const vector<LloydSite> & sites, vector<LloydSample> & points) const
{
    throw Exception("DiscreteLloydVoronoi::Get(): shouldn't be called");
    return 0;
}
