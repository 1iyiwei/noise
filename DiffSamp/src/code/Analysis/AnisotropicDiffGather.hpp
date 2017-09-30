/*
  AnisotropicDiffGather.hpp

  diff gather for anisotropic sampling

  Li-Yi Wei
  10/20/2010

*/

#ifndef _ANISOTROPIC_DIFF_GATHER_HPP
#define _ANISOTROPIC_DIFF_GATHER_HPP

#include <string>
using namespace std;

#include "DiffGather.hpp"

class AnisotropicDiffGather : public DiffGather
{
public:
    virtual ~AnisotropicDiffGather(void) = 0;

    virtual string Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const;
};

#endif
