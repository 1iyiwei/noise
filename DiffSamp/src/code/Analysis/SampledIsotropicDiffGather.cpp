/*
  SampledIsotropicDiffGather.cpp

  Li-Yi Wei
  09/14/2010

*/

#include <algorithm>
using namespace std;

#include <math.h>
#include "SampledIsotropicDiffGather.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

SampledIsotropicDiffGather::SampledIsotropicDiffGather(const SampledDistanceField & distance_field, const float max_relative_dd, const float local_diff_range, const DiffNorm diff_norm, const DiffWeight diff_weight, const int max_similarity_cache_length) : _distance_field(distance_field), _max_relative_dd(max_relative_dd), _local_diff_range(local_diff_range), _diff_norm(diff_norm), _diff_weight(diff_weight), _max_cache_length(max_similarity_cache_length), _grid(distance_field.GetDomain(), distance_field.DataSize(), distance_field.CellSize()), _visit(distance_field.DataSize())
{
    // nothing else to do
}

SampledIsotropicDiffGather::~SampledIsotropicDiffGather(void)
{
    // nothing else to do
}

int SampledIsotropicDiffGather::Set(const vector<Sample> & samples)
{
    _grid.Clear();

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!_grid.Add(samples[i]))
        {
            return 0;
        }
    }

    return 1;
}

string SampledIsotropicDiffGather::Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const
{
    diffs.clear();

    // find anchor location
    vector<int> anchor_index;
    if(! _grid.LocateCell(anchor, anchor_index))
    {
        return "SampledIsotropicDiffGather::Get(): cannot locate anchor in grid";
    }

    const int dimension = _distance_field.Dimension();
    const float anchor_dist = _distance_field.Query(anchor);
    const float mean_dist = _distance_field.Mean();

    // see if in cache
    bool in_cache = 0;
    Neighborhood element;
    for(unsigned int i = 0; i < _cache.size(); i++)
    {
        if(_cache[i].center == anchor_index)
        {
            in_cache = 1;

            element = _cache[i];

            // update cache
            _cache.erase(_cache.begin() + i);
            _cache.push_back(element);
        }
    }

    if(! in_cache)
    {
        element.center = anchor_index;
        if(! Find(_distance_field, _max_relative_dd, _local_diff_range, element.center, element.neighbors))
        {
            return "SampledIsotropicDiffGather::Get(): cannot find neighbors";
        }

        _cache.push_back(element);

        if(_cache.size() > _max_cache_length)
        {
            _cache.pop_front();
        }
    }

    // element contains the crucial information
    vector<const Sample *> neighbor_samples;
    Diff diff;

    for(unsigned int k = 0; k < element.neighbors.size(); k++)
    {
        if(! _grid.GetSamples(element.neighbors[k], neighbor_samples))
        {
            return "SampledIsotropicDiffGather::Get(): cannot get grid samples";
        }

        for(unsigned int i = 0; i < neighbor_samples.size(); i++)
        {
            const Sample & neighbor = *neighbor_samples[i];

            if(allow_self_pair || (&anchor != &neighbor))
            {
                const float neighbor_dist = _distance_field.Query(neighbor);

                // diff norm
                float norm_factor = 1;

                if(_diff_norm == ME_N)
                {
                    norm_factor = mean_dist/anchor_dist;
                }
                else if(_diff_norm == OTHER_N)
                {
                    norm_factor = mean_dist/neighbor_dist;
                }
                else if(_diff_norm == SYMMETRY_N)
                {
                    norm_factor = 2*mean_dist/(anchor_dist + neighbor_dist);
                }
                else
                {
                    norm_factor = 1;
                }

                diff.diff = Utility::Diff(anchor.coordinate, neighbor.coordinate);

                // diff weight
                if(_diff_weight == OTHER_ME_W)
                {
                    diff.weight = pow(1.0*neighbor_dist/anchor_dist, 1.0*dimension);
                }
                else if(_diff_weight == OTHER_MEAN_ME_MEAN_W)
                {
                    diff.weight = pow(1.0*anchor_dist*neighbor_dist/(mean_dist*mean_dist), 1.0*dimension);
                }
                else
                {
                    diff.weight = 1.0;
                }

                const float min_dist = _local_diff_range*min(anchor_dist, neighbor_dist);
                if((_local_diff_range < 0) || (Utility::Norm2(diff.diff) <= (min_dist*min_dist)))
                {
                    diff.diff = Utility::Multiply(diff.diff, norm_factor);
                    diffs.push_back(diff);
                }
            }
        }
    }

    return "";
}

int SampledIsotropicDiffGather::Find(const SampledDistanceField & distance_field, const float max_relative_dd, const float local_diff_range, const vector<int> & query, vector< vector<int> > & output) const
{
    output.clear();
    _visit.Clear(0);
    deque< vector<int> > active_list;

    output.push_back(query);
    if(! _visit.Put(query, 1)) throw Exception("SampledIsotropicDiffGather::Find(): can't put visit");
    active_list.push_back(query);

    const float cell_size = distance_field.CellSize();

    const float query_dist = distance_field.Query(query);

    vector<int> current, neighbor;
    bool neighbor_visit = 0;
    float neighbor_dist = 0;
    while(active_list.size() > 0)
    {
        current = active_list[0];
        active_list.pop_front();

        neighbor = current;
        for(unsigned int dim = 0; dim < neighbor.size(); dim++)
            for(int dir = -1; dir <= 1; dir += 2)
            {
                neighbor = current;
                neighbor[dim] += dir;

                if(_visit.Get(neighbor, neighbor_visit))
                {
                    neighbor_dist = distance_field.Query(neighbor);

                    if(!neighbor_visit && ((max_relative_dd < 0) || (fabs(neighbor_dist - query_dist) <= max_relative_dd*min(query_dist, neighbor_dist))) && ((local_diff_range < 0) || (MinDistance(query, neighbor, cell_size)) <= local_diff_range*min(query_dist, neighbor_dist)))
                    {
                        // add neighbor
                        output.push_back(neighbor);
                        if(! _visit.Put(neighbor, 1)) throw Exception("SampledIsotropicDiffGather::Find(): can't put visit");
                        active_list.push_back(neighbor);
                    }
                }
            }
    }

    // done
    return 1;
}
