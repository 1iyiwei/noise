/*
  VoronoiFairClassWeight.hpp

  weigh class samples so that each Voronoi region has same/similar weight

  Li-Yi Wei
  09/10/2009

*/

#ifndef _VORONOI_FAIR_CLASS_WEIGHT_HPP
#define _VORONOI_FAIR_CLASS_WEIGHT_HPP

#include "SampleCountClassWeight.hpp"

class VoronoiFairClassWeight : public SampleCountClassWeight
{
public:
    VoronoiFairClassWeight(const vector<int> & num_samples_per_class);
    ~VoronoiFairClassWeight(void);

    float Get(const int class_id0, const int class_id1) const;

    float Get(const vector<int> & source_classes) const;
};
#endif
