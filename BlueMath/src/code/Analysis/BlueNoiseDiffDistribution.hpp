/*
  BlueNoiseDiffDistribution.hpp

  Li-Yi Wei
  11/04/2010

*/

#ifndef _BLUE_NOISE_DIFF_DISTRIBUTION_HPP
#define _BLUE_NOISE_DIFF_DISTRIBUTION_HPP

#include "DiffDistribution.hpp"

class BlueNoiseDiffDistribution : public DiffDistribution
{
public:
    BlueNoiseDiffDistribution(const vector<float> & diff_domain_min, const vector<float> & diff_domain_max, const float radius);
    BlueNoiseDiffDistribution(const vector<float> & diff_domain_min, const vector<float> & diff_domain_max, const float radius, const float total);

    virtual ~BlueNoiseDiffDistribution(void);

    float Query(const Diff & query) const;

protected:
    const float _radius2;
    float _value; // outside the low frequency hole
};

#endif
