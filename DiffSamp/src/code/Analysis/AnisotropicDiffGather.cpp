/*
  AnisotropicDiffGather.cpp

  Li-Yi Wei
  09/14/2010

*/

#include "AnisotropicDiffGather.hpp"
#include "Exception.hpp"

AnisotropicDiffGather::~AnisotropicDiffGather(void)
{
    // nothing else to do
}

string AnisotropicDiffGather::Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const
{
    throw Exception("AnisotropicDiffGather::Get(): shouldn't be called!");

    return "";
}
