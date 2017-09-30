/*
  IsotropicDiffGather.cpp

  Li-Yi Wei
  09/14/2010

*/

#include "IsotropicDiffGather.hpp"
#include "Exception.hpp"

IsotropicDiffGather::~IsotropicDiffGather(void)
{
    // nothing else to do
}

string IsotropicDiffGather::Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const
{
    throw Exception("IsotropicDiffGather::Get(): shouldn't be called!");

    return "";
}
