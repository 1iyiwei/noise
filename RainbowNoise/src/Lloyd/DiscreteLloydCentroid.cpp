/*
  DiscreteLloydCentroid.cpp

  Li-Yi Wei
  07/16/2009

*/

#include "DiscreteLloydCentroid.hpp"
#include "Exception.hpp"

DiscreteLloydCentroid::DiscreteLloydCentroid(const Domain & domain, const ClassWeight & class_weight) : _domain(domain), _class_weight(class_weight), _snap(1)
{
    // nothing else to do
}

DiscreteLloydCentroid::~DiscreteLloydCentroid(void)
{
    // nothing else to do
}

int DiscreteLloydCentroid::NumClass(void) const
{
    return _class_weight.NumClass();
}

float DiscreteLloydCentroid::Get(const vector<LloydSample> & points, vector<LloydSite> & sites) const
{
    throw Exception("DiscreteLloydCentroid::Get(): shouldn't be called");
    return -1;
}

int DiscreteLloydCentroid::Snap(const vector<LloydSample> & points, vector<LloydSite> & sites) const
{
    throw Exception("DiscreteLloydCentroid::Snap(): shouldn't be called");
    return 0;
}
   
void DiscreteLloydCentroid::Snap(const bool option)
{
    _snap = option;
}

bool DiscreteLloydCentroid::Snap(void) const
{
    return _snap;
}

int DiscreteLloydCentroid::Adopt(vector<LloydSite> & sites) const
{
    for(unsigned int i = 0; i < sites.size(); i++)
    {
        if(! _domain.Adopt(sites[i].sample, sites[i].sample))
        {
            return 0;
        }
    }

    return 1;
}
