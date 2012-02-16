/*
  IsotropicDeformer.cpp

  Li-Yi Wei
  08/26/2008

*/

#include <algorithm>
using namespace std;

#include "IsotropicDeformer.hpp"

// make sure the following is consistent with KDTree.cpp under BlueNoise src
#define _USE_MEAN_NOT_MAX_METRIC

IsotropicDeformer::IsotropicDeformer(const DistanceField & field, const float global_mean) : _distance_field(field), _global_mean(global_mean)
{
    // nothing to do
}

IsotropicDeformer::~IsotropicDeformer(void)
{
    // nothing to do
}

float IsotropicDeformer::Query(const Sample & s1, const Sample & s2) const
{
#ifdef _USE_MEAN_NOT_MAX_METRIC
    return 2*_global_mean/(_distance_field.Query(s1) + _distance_field.Query(s2));
#else
    return _global_mean/max(_distance_field.Query(s1), _distance_field.Query(s2));
#endif
}

