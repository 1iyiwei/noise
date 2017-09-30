/*
  DiffDistribution.cpp

  Li-Yi Wei
  11/04/2010

*/

#include "DiffDistribution.hpp"
#include "Exception.hpp"

DiffDistribution::DiffDistribution(const vector<float> & diff_domain_min, const vector<float> & diff_domain_max) : _diff_domain_min(diff_domain_min), _diff_domain_max(diff_domain_max)
{
    if(diff_domain_min.size() != diff_domain_max.size())
    {
        throw Exception("DiffDistribution::DiffDistribution(): incompatible dimension");
    }
}

DiffDistribution::~DiffDistribution(void)
{
    // nothing else to do
}

int DiffDistribution::Dimension(void) const
{
    return _diff_domain_min.size();
}

float DiffDistribution::Query(const Diff & query) const
{
    throw Exception("DiffDistribution::Query(): shouldn't be called!");
    return 0;
}
