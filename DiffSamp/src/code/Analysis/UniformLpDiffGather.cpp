/*
  UniformLpDiffGather.cpp

  Li-Yi Wei
  09/08/2010

*/

#include <math.h>

#include "UniformLpDiffGather.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Exception.hpp"

UniformLpDiffGather::UniformLpDiffGather(const float p, const float local_diff_range, const float mean_dist, const DiffNorm diff_norm, const DiffWeight diff_weight): _p(p), _local_diff_range(local_diff_range), _mean_dist(mean_dist), _diff_norm(diff_norm), _diff_weight(diff_weight)
{
    // nothing else to do
}

UniformLpDiffGather::~UniformLpDiffGather(void)
{
    // nothing else to do
}

string UniformLpDiffGather::Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const
{
    diffs.clear();

    const float max_dist = _local_diff_range >= 0 ? _local_diff_range*_mean_dist*sqrt(anchor.coordinate.Dimension()*1.0) : Math::INF;

    Diff diff;

    for(unsigned int i = 0; i < _p_samples.size(); i++)
    {
        if(allow_self_pair || (&anchor != _p_samples[i]))
        {
            const float l2_dist = pow(Utility::Distance2(anchor.coordinate, _p_samples[i]->coordinate)*1.0, 1.0/2);

            const float lp_dist = Math::IsINF(_p) ? Utility::DistanceINF(anchor.coordinate, _p_samples[i]->coordinate) : pow(Utility::Distancep(_p, anchor.coordinate, _p_samples[i]->coordinate)*1.0, 1.0/_p);

            const float ratio = l2_dist > 0 ? lp_dist/l2_dist : 1.0;

            if(_diff_norm == UNIT_N)
            {
                diff.diff = Utility::Diff(anchor.coordinate, _p_samples[i]->coordinate);
            }
            else
            {
                diff.diff = Utility::Multiply(Utility::Diff(anchor.coordinate, _p_samples[i]->coordinate), ratio);
            }

            if(_diff_weight == UNIT_W)
            {
                diff.weight = 1.0;
            }
            else if(_diff_weight == OTHER_ME_W)
            {
                diff.weight = ratio;
            }
            else
            {
                diff.weight = (Random::UniformRandom() < ratio);
            }

            if(l2_dist <= max_dist)
            {
                diffs.push_back(diff);
            }
        }
    }

    return "";
}
