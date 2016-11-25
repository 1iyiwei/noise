/*
  SampledIsotropicDiffGather.hpp

  diff gather for isotropic sampling with sampled distance field

  Li-Yi Wei
  09/08/2010

*/

#ifndef _SAMPLED_ISOTROPIC_DIFF_GATHER_HPP
#define _SAMPLED_ISOTROPIC_DIFF_GATHER_HPP

#include <deque>
using namespace std;

#include "IsotropicDiffGather.hpp"
#include "SampledDistanceField.hpp"
#include "GridSampleHolder.hpp"

class SampledIsotropicDiffGather : public IsotropicDiffGather
{
public:
    // max_relative_dd: maximum relative distance difference, i.e. for each query sample s with r(s), only return diff pairs with s' for which abs(r(s)-r(s')) <= max_relative_dd*r(s)
    // local_diff_range: only collect diff within local_diff_range*r(s)
    SampledIsotropicDiffGather(const SampledDistanceField & distance_field, const float max_relative_dd, const float local_diff_range, const DiffNorm diff_norm, const DiffWeight diff_weight, const int max_similarity_cache_length);

    virtual ~SampledIsotropicDiffGather(void);

    int Set(const vector<Sample> & samples);

    string Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const;

protected:
    // find similarity sets for a query
    int Find(const SampledDistanceField & distance_field, const float max_relative_dd, const float local_diff_range, const vector<int> & query, vector< vector<int> > & output) const;

protected:
    const SampledDistanceField & _distance_field;
    const float _max_relative_dd;
    const float _local_diff_range;
    const DiffNorm _diff_norm;
    const DiffWeight _diff_weight;
    const int _max_cache_length;

    GridSampleHolder _grid;

    // record all neighbors with sufficiently similar distance in center
    struct Neighborhood
    {
        vector<int> center;
        vector< vector<int> > neighbors;
    };

    mutable deque<Neighborhood> _cache;
    mutable Array<bool> _visit;
};

#endif
