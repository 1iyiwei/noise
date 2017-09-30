/*
  DiffDistribution.hpp

  the base class of all differential distributions

  Li-Yi Wei
  11/04/2010

*/

#ifndef _DIFF_DISTRIBUTION_HPP
#define _DIFF_DISTRIBUTION_HPP

#include <vector>
using namespace std;

#include "DiffGather.hpp"

class DiffDistribution
{
public:
    typedef DiffGather::Diff Diff;

    DiffDistribution(const vector<float> & diff_domain_min, const vector<float> & diff_domain_max);
    virtual ~DiffDistribution(void) = 0;

    int Dimension(void) const;

    // answer the distribution value for the query diff
    virtual float Query(const Diff & query) const;

protected:
    const vector<float> _diff_domain_min, _diff_domain_max;
};

#endif
