/*
  DiffGather.hpp

  the base class for all diff gather

  Li-Yi Wei
  09/08/2010

*/

#ifndef _DIFF_GATHER_HPP
#define _DIFF_GATHER_HPP

#include <vector>
#include <string>
using namespace std;

#include "Sample.hpp"

class DiffGather
{
public:
    // options to tune the length of diff: no adjust, me only, me and neighbor
    enum DiffNorm {UNIT_N, ME_N, OTHER_N, SYMMETRY_N};

    // options to weight a diff
    enum DiffWeight {UNIT_W, OTHER_ME_W, OTHER_MEAN_ME_MEAN_W};

    virtual ~DiffGather(void) = 0;

    virtual int Set(const vector<Sample> & samples);

    struct Diff
    {
        Diff(void): weight(1) {};
        Coordinate diff;
        float weight;
    };

    // get all diff pairs with one end in anchor
    // return "" if successful, error message else
    virtual string Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const;

    static DiffNorm TranslateNorm(const string diff_norm_option);
    static DiffWeight TranslateWeight(const string diff_norm_option);

protected:
    // diff with minimum possible distance between any pair of samples in 2 cells
    static vector<int> MinDiff(const vector<int> & index0, const vector<int> & index1);

    // minimum possible distance between any pair of samples in 2 cells
    static float MinDistance(const vector<int> & index0, const vector<int> & index1, const float cell_size);

protected:
    vector<const Sample *> _p_samples;
};

#endif
