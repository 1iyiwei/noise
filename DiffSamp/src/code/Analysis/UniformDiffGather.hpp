/*
  UniformDiffGather.hpp

  diff gather for uniform sampling

  Li-Yi Wei
  09/08/2010

*/

#ifndef _UNIFORM_DIFF_GATHER_HPP
#define _UNIFORM_DIFF_GATHER_HPP

#include "DiffGather.hpp"

class UniformDiffGather : public DiffGather
{
public:
    virtual ~UniformDiffGather(void);

    virtual string Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const;
};

#endif
