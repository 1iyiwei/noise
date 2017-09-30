/*
  UniformDiffGather.cpp

  Li-Yi Wei
  09/08/2010

*/

#include "UniformDiffGather.hpp"
#include "Utility.hpp"

UniformDiffGather::~UniformDiffGather(void)
{
    // nothing else to do
}

string UniformDiffGather::Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const
{
    diffs.clear();

    Diff diff;

    for(unsigned int i = 0; i < _p_samples.size(); i++)
    {
        if(allow_self_pair || (&anchor != _p_samples[i]))
        {
            diff.diff = Utility::Diff(anchor.coordinate, _p_samples[i]->coordinate);
            diffs.push_back(diff);
        }
    }

    return "";
}
