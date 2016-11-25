/*
  BlueNoiseDiffDistribution.cpp

  Li-Yi Wei
  11/04/2010

*/

#include <math.h>

#include "BlueNoiseDiffDistribution.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "Exception.hpp"

BlueNoiseDiffDistribution::BlueNoiseDiffDistribution(const vector<float> & diff_domain_min, const vector<float> & diff_domain_max, const float radius) : DiffDistribution(diff_domain_min, diff_domain_max), _radius2(radius*radius), _value(1.0)
{
    // nothing else to do
}

BlueNoiseDiffDistribution::BlueNoiseDiffDistribution(const vector<float> & diff_domain_min, const vector<float> & diff_domain_max, const float radius, const float total) : DiffDistribution(diff_domain_min, diff_domain_max), _radius2(radius*radius), _value(1.0)
{
    for(unsigned int i = 0; i < diff_domain_min.size(); i++)
    {
        if(-fabs(radius) < diff_domain_min[i]) throw Exception("BlueNoiseDiffDistribution::BlueNoiseDiffDistribution(): protruding inner region");
    }

    for(unsigned int i = 0; i < diff_domain_max.size(); i++)
    {
        if(fabs(radius) > diff_domain_max[i]) throw Exception("BlueNoiseDiffDistribution::BlueNoiseDiffDistribution(): protruding inner region");
    }

    float total_volume = 1;
    for(unsigned int i = 0; i < diff_domain_min.size(); i++)
    {
        total_volume *= (diff_domain_max[i] - diff_domain_min[i]);
    }

    total_volume -= Math::HyperSphereVolume(Dimension(), fabs(radius));

    _value = total/(total_volume);
}

BlueNoiseDiffDistribution::~BlueNoiseDiffDistribution(void)
{
    // nothing else to do
}

float BlueNoiseDiffDistribution::Query(const Diff & query) const
{
    const float norm2 = Utility::Norm2(query.diff);

    if(norm2 < _radius2)
    {
        return 0;
    }
    else
    {
        return _value;
    }
}
