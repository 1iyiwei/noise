/*
  DiscreteDiffDistribution.cpp

  Li-Yi Wei
  11/04/2010

*/

#include <math.h>

#include "DiscreteDiffDistribution.hpp"
#include "Exception.hpp"

DiscreteDiffDistribution::DiscreteDiffDistribution(const Array<float> & distribution, const float cell_size, const vector<float> & diff_domain_min) : DiffDistribution(diff_domain_min, DiffDomainMax(distribution, cell_size, diff_domain_min)), _distribution(distribution), _cell_size(cell_size)
{
    // nothing else to do
}

DiscreteDiffDistribution::~DiscreteDiffDistribution(void)
{
    // nothing else to do
}

float DiscreteDiffDistribution::Query(const Diff & query) const
{
    if(query.diff.Dimension() != Dimension())
    {
        throw Exception(" DiscreteDiffDistribution::Query(): dimension mismatch");
    }

    vector<int> index(query.diff.Dimension());

    for(unsigned int i = 0; i < index.size(); i++)
    {
        index[i] = floor((query.diff[i] - _diff_domain_min[i])/_cell_size);
    }

    float value = 0;
    if(! _distribution.Get(index, value))
    {
        throw Exception(" DiscreteDiffDistribution::Query(): cannot get value");
    }

    return value;
}

vector<float> DiscreteDiffDistribution::DiffDomainMax(const Array<float> & distribution, const float cell_size, const vector<float> & diff_domain_min)
{
    if(distribution.Dimension() != diff_domain_min.size())
    {
        throw Exception(" DiscreteDiffDistribution::DiffDomainMax(): dimension mismatch");
    }

    vector<float> diff_domain_max(diff_domain_min);

    for(unsigned int i = 0; i < diff_domain_max.size(); i++)
    {
        diff_domain_max[i] = diff_domain_min[i] + cell_size*distribution.Size(i);
    }

    return diff_domain_max;
}
