/*
  IsotropicDiffGather.hpp

  diff gather for isotropic sampling

  Li-Yi Wei
  09/14/2010

*/

#ifndef _ISOTROPIC_DIFF_GATHER_HPP
#define _ISOTROPIC_DIFF_GATHER_HPP

#include <string>
using namespace std;

#include "DiffGather.hpp"

class IsotropicDiffGather : public DiffGather
{
public:
    virtual ~IsotropicDiffGather(void) = 0;

    virtual string Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const;
};

#endif
