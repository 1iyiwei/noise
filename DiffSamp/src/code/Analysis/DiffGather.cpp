/*
  DiffGather.cpp

  Li-Yi Wei
  09/08/2010

*/

#include <math.h>

#include "DiffGather.hpp"
#include "Exception.hpp"

DiffGather::~DiffGather(void)
{
    // nothing else to do
}

int DiffGather::Set(const vector<Sample> & samples)
{
    _p_samples = vector<const Sample *>(samples.size());

    for(unsigned int i = 0; i < _p_samples.size(); i++)
    {
        _p_samples[i] = &samples[i];
    }

    return 1;
}

string DiffGather::Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const
{
    throw Exception("DiffGather::Get() shouldn't be called");

    return "";
}

DiffGather::DiffNorm DiffGather::TranslateNorm(const string diff_norm_option)
{
    if(diff_norm_option == "me")
    {
        return ME_N;
    }
    else if(diff_norm_option == "other")
    {
        return OTHER_N;
    }
    else if(diff_norm_option == "symmetry")
    {
        return SYMMETRY_N;
    }
    else
    {
        return UNIT_N;
    }
}

DiffGather::DiffWeight DiffGather::TranslateWeight(const string diff_weight_option)
{
    if(diff_weight_option == "otherme")
    {
        return OTHER_ME_W;
    }
    else if(diff_weight_option == "othermeanmemean")
    {
        return OTHER_MEAN_ME_MEAN_W;
    }
    else
    {
        return UNIT_W;
    }
}

vector<int> DiffGather::MinDiff(const vector<int> & index0, const vector<int> & index1)
{
    if(index0.size() != index1.size()) throw Exception("SampledIsotropicDiffGather::MinDiff(): index0.size() != index1.size()");

    vector<int> output(index0);

    for(unsigned int i = 0; i < output.size(); i++)
    {
        output[i] = index0[i] - index1[i];
        
        if(output[i] > 0)
        {
            output[i] -= 1;
        }
        else if(output[i] < 0)
        {
            output[i] += 1;
        }
        else
        {
            // do nothing
        }
    }

    return output;
}

float DiffGather::MinDistance(const vector<int> & index0, const vector<int> & index1, const float cell_size)
{
    if(index0.size() != index1.size()) throw Exception("SampledIsotropicDiffGather::MinDistance(): index0.size() != index1.size()");

    float distance = 0;

#if 0
    vector<int> diff = MinDiff(index0, index1, cell_size);

    for(unsigned int i = 0; i < diff.size(); i++)
    {
        distance += diff[i]*diff[i];
    }
#else
    for(unsigned int i = 0; i < index0.size(); i++)
    {
        float diff = fabs(1.0*(index0[i] - index1[i]));
        if(diff > 0) diff -= 1;

        distance += diff*diff;
    }
#endif

    return sqrt(distance)*cell_size;
}
