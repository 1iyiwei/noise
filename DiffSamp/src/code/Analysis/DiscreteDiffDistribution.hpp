/*
  DiscreteDiffDistribution.hpp

  Li-Yi Wei
  11/04/2010

*/

#ifndef _DISCRETE_DIFF_DISTRIBUTION_HPP
#define _DISCRETE_DIFF_DISTRIBUTION_HPP

#include "DiffDistribution.hpp"
#include "Array.hpp"

class DiscreteDiffDistribution : public DiffDistribution
{
public:
    DiscreteDiffDistribution(const Array<float> & distribution, const float cell_size, const vector<float> & diff_domain_min);
    virtual ~DiscreteDiffDistribution(void);
   
    float Query(const Diff & query) const;

protected:
    static vector<float> DiffDomainMax(const Array<float> & distribution, const float cell_size, const vector<float> & diff_domain_min);

protected:
    const Array<float> _distribution;
    const float _cell_size;
};

#endif
