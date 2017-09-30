/*
  UniformLpDiffGather.hpp

  diff gather for uniform Lp sampling

  Li-Yi Wei
  04/23/2011

*/

#ifndef _UNIFORM_LP_DIFF_GATHER_HPP
#define _UNIFORM_LP_DIFF_GATHER_HPP

#include "DiffGather.hpp"

class UniformLpDiffGather : public DiffGather
{
public:
    UniformLpDiffGather(const float p, const float local_diff_range, const float mean_dist, const DiffNorm diff_norm, const DiffWeight diff_weight);
    virtual ~UniformLpDiffGather(void);

    virtual string Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const;

protected:
    const float _p;
    const float _local_diff_range;
    const float _mean_dist;
    const DiffNorm _diff_norm;
    const DiffWeight _diff_weight;
};

#endif
