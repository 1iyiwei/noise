/*
  HybridDiffGenerator.cpp

  Li-Yi Wei
  11/04/2008

*/

#include <math.h>

#include "HybridDiffGenerator.hpp"
#include "Random.hpp"

HybridDiffGenerator::HybridDiffGenerator(const vector<float> & domain_size, const float r_value, const float r_variance) : DiffGenerator(domain_size), _r_value(fabs(r_value)), _r_variance(fabs(r_variance)), _keep_diff_less_r(r_value < 0), _spawn_diff_less_r(r_variance >= 0), _coord0(domain_size.size()), _coord1(_coord0)
{
    // nothing to do
}

HybridDiffGenerator::~HybridDiffGenerator(void)
{
    // nothing to do
}

int HybridDiffGenerator::Produce(vector<Sample> & differences) const
{
    vector<Sample> results;
    
    unsigned int i = 0;
    while(i < differences.size())
    {
        const int count = UnitProduce(results);

        for(unsigned int j = 0; (i < differences.size()) && (j < results.size()); i++, j++)
        {
            differences[i] = results[j];
        }
    }

    return 1;
}

int HybridDiffGenerator::UnitProduce(vector<Sample> & results) const
{
    for(int i = 0; i < _coord0.Dimension(); i++)
    {
        _coord0[i] = Random::UniformRandom();
        _coord1[i] = Random::UniformRandom();
    }

    Sample difference;
    float diff_length = 0;
    difference.coordinate = _coord0;
    for(int i = 0; i < difference.coordinate.Dimension(); i++)
    {
        difference.coordinate[i] = (_coord0[i] - _coord1[i])*_domain_size[i];
        diff_length += difference.coordinate[i]*difference.coordinate[i];
    }

    diff_length = sqrt(diff_length);

    results.clear();

    if(_keep_diff_less_r || (diff_length >= _r_value))
    {
        results.push_back(difference);
    }

    if(_spawn_diff_less_r && (diff_length < _r_value))
    {
#if 1
        const float new_length = _r_value*(1.0 + fabs(Random::GaussianRandom())*_r_variance);
#else
        const float foo = Random::UniformRandom()*_r_value/_r_variance;
        const float new_length = _r_value*(1.0 + (_r_variance > 0 ? exp(-foo*foo) : 0));
#endif
        for(int i = 0; i < difference.coordinate.Dimension(); i++)
        {
            difference.coordinate[i] *= new_length/diff_length;
        }

        results.push_back(difference);
    }

    return results.size();
}
