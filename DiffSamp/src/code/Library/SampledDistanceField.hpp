/*
  SampledDistanceField.hpp

  Li-Yi Wei
  07/15/2007

  08/26/2008
  modified for multi-class sampling

  09/14/2010
  modify back for single-class

*/

#ifndef _SAMPLED_DISTANCE_FIELD
#define _SAMPLED_DISTANCE_FIELD

#include "DistanceField.hpp"
#include "SampledImportanceField.hpp"

class SampledDistanceField : public DistanceField
{
public: 
    // average_min_distance: the average min-spacing in Poisson disk sampling
    SampledDistanceField(const Domain & domain, const SampledImportanceField & importance, const float average_min_distance);
    SampledDistanceField(const Domain & domain, const Array<float> & importance, const float cell_size, const float average_min_distance);
    ~SampledDistanceField(void);

    float Query(const Sample & sample) const;

    float Mean(void) const;

    // max distance
    float Max(void) const;

    // class specific functions below

    // the (sampled) data size
    const vector<int> & DataSize(void) const;
    float CellSize(void) const;

    float Query(const vector<int> & index) const;

    static vector<float> DomainSize(const Array<float> & importance, const float cell_size);

protected:
    static int CommonCheck(const Domain & domain, const vector<float> & domain_size);

    void CommonConstruct(const float average_min_distance);

protected:
    const SampledImportanceField _importance;
    Array<float> _data;
    const float _mean_distance;
    float _max_distance;
    mutable vector<int> _index; // used in Query to avoid repeated memoc
};

#endif
