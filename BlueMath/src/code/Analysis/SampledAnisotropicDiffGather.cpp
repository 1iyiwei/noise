/*
  SampledAnisotropicDiffGather.cpp

  Li-Yi Wei
  10/20/2010

*/

#include <algorithm>
using namespace std;

#include <math.h>
#include "SampledAnisotropicDiffGather.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

SampledAnisotropicDiffGather::SampledAnisotropicDiffGather(const JacobianField & jacobian_field, const DomainSpec & grid_spec, const float max_relative_dd, const float local_diff_range, const DiffNorm diff_norm, const DiffWeight diff_weight, const int max_similarity_cache_length) : _jacobian_field(jacobian_field), _grid_spec(grid_spec), _max_relative_dd(max_relative_dd), _local_diff_range(local_diff_range), _diff_norm(diff_norm), _diff_weight(diff_weight), _max_cache_length(max_similarity_cache_length), _grid(jacobian_field.GetDomain(), grid_spec), _visit(grid_spec.domain_size)
{
    // nothing else to do
}

SampledAnisotropicDiffGather::SampledAnisotropicDiffGather(const DiscreteJacobianField & jacobian_field, const float max_relative_dd, const float local_diff_range, const DiffNorm diff_norm, const DiffWeight diff_weight, const int max_similarity_cache_length) : _jacobian_field(jacobian_field), _grid_spec(jacobian_field.DataSize(), jacobian_field.CellSize()), _max_relative_dd(max_relative_dd), _local_diff_range(local_diff_range), _diff_norm(diff_norm), _diff_weight(diff_weight), _max_cache_length(max_similarity_cache_length), _grid(jacobian_field.GetDomain(), _grid_spec), _visit(_grid_spec.domain_size)
{
    // nothing else to do
}

SampledAnisotropicDiffGather::~SampledAnisotropicDiffGather(void)
{
    // nothing else to do
}

int SampledAnisotropicDiffGather::Set(const vector<Sample> & samples)
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

string SampledAnisotropicDiffGather::Get(const Sample & anchor, const int allow_self_pair, vector<Diff> & diffs) const
{
    diffs.clear();

    // find anchor location
    vector<int> anchor_index;
    if(! _grid.LocateCell(anchor, anchor_index))
    {
        return "SampledAnisotropicDiffGather::Get(): cannot locate anchor in grid";
    }

    const int dimension = _jacobian_field.Dimension();

    Matrix anchor_jacobian;
    if(! _jacobian_field.Inverse(anchor, anchor_jacobian))
    {
        return "SampledAnisotropicDiffGather::Get(): cannot get anchor jacobian";
    }

    const float mean_dist = _jacobian_field.ForwardMeanEigenValue();

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
        if(! Find(_jacobian_field, _max_relative_dd, _local_diff_range, element.center, element.neighbors))
        {
            return "SampledAnisotropicDiffGather::Get(): cannot find neighbors";
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
    Coordinate anchor_diff, neighbor_diff;

    for(unsigned int k = 0; k < element.neighbors.size(); k++)
    {
        if(! _grid.GetSamples(element.neighbors[k], neighbor_samples))
        {
            return "SampledAnisotropicDiffGather::Get(): cannot get grid samples";
        }

        for(unsigned int i = 0; i < neighbor_samples.size(); i++)
        {
            const Sample & neighbor = *neighbor_samples[i];

            if(allow_self_pair || (&anchor != &neighbor))
            {
                Matrix neighbor_jacobian;
                if(! _jacobian_field.Inverse(neighbor, neighbor_jacobian))
                {
                    return "SampledAnisotropicDiffGather::Get(): cannot get neighbor jacobian";
                }

                diff.diff = Utility::Diff(anchor.coordinate, neighbor.coordinate);

                // diff weight
                if(_diff_weight == OTHER_ME_W)
                {
                    // diff.weight = pow(1.0*neighbor_dist/anchor_dist, 1.0*dimension);
                     return "SampledAnisotropicDiffGather::Get(): diff_weight == OTHER_ME_W not yet supported";
                }
                else if(_diff_weight == OTHER_MEAN_ME_MEAN_W)
                {
                    // diff.weight = pow(1.0*anchor_dist*neighbor_dist/(mean_dist*mean_dist), 1.0*dimension);
                     return "SampledAnisotropicDiffGather::Get(): diff_weight == OTHER_MEAN_ME_MEAN_W not yet supported";
                }
                else
                {
                    diff.weight = 1.0;
                }

                // diff norm
                Matrix norm_factor(anchor_jacobian);

                if(_diff_norm == ME_N)
                {
                    if(! anchor_jacobian.Multiply(mean_dist, norm_factor)) throw Exception("SampledAnisotropicDiffGather::Get(): error in anchor_jacobian.Multiply(mean_dist, norm_factor)");
                }
                else if(_diff_norm == OTHER_N)
                { 
                    if(! neighbor_jacobian.Multiply(mean_dist, norm_factor)) throw Exception("SampledAnisotropicDiffGather::Get(): error in neighbor_jacobian.Multiply(mean_dist, norm_factor)");
                }
                else if(_diff_norm == SYMMETRY_N)
                {
                    // hack! I am averaging inverse jacobian
                    // not the inverse of the average of forward jacobian
                    if(! anchor_jacobian.Add(neighbor_jacobian, norm_factor)) throw Exception("SampledAnisotropicDiffGather::Get(): error in anchor_jacobian.Add(neighbor_jacobian, norm_factor)");

                    if(! norm_factor.Multiply(0.5)) throw Exception("SampledAnisotropicDiffGather::Get(): error in norm_factor.Multiply(0.5)");

                    if(! norm_factor.Multiply(mean_dist, norm_factor)) throw Exception("SampledAnisotropicDiffGather::Get(): norm_factor.Multiply(mean_dist, norm_factor)");
                }
                else
                {
                    if(!norm_factor.Identity()) throw Exception("SampledAnisotropicDiffGather::Get(): cannot make norm_factor an identity matrix");
                }

                if(! anchor_jacobian.Multiply(diff.diff, anchor_diff)) throw Exception("SampledAnisotropicDiffGather::Get(): error in anchor_jacobian.Multiply(diff.diff, anchor_diff)");

                if(! neighbor_jacobian.Multiply(diff.diff, neighbor_diff)) throw Exception("SampledAnisotropicDiffGather::Get(): error in neighbor_jacobian.Multiply(diff.diff, neighbor_diff)");
                
                const float anchor_diff2 = Utility::Norm2(anchor_diff);
                const float neighbor_diff2 = Utility::Norm2(neighbor_diff);

                if((_local_diff_range < 0) || (max(anchor_diff2, neighbor_diff2) <= _local_diff_range*_local_diff_range))
                {
                    if(! norm_factor.Multiply(diff.diff, diff.diff)) throw Exception("SampledAnisotropicDiffGather::Get(): error in norm_factor.Multiply(diff.diff, diff.diff)");

                    diffs.push_back(diff);
                }
            }
        }
    }

    return "";
}

int SampledAnisotropicDiffGather::Find(const JacobianField & jacobian_field, const float max_relative_dd, const float local_diff_range, const vector<int> & query, vector< vector<int> > & output) const
{
    if(max_relative_dd >= 0) throw Exception("SampledAnisotropicDiffGather::Find(): only negative max_relative_dd supported right now!");

    output.clear();
    _visit.Clear(0);
    deque< vector<int> > active_list;

    output.push_back(query);
    if(! _visit.Put(query, 1)) throw Exception("SampledAnisotropicDiffGather::Find(): can't put visit");
    active_list.push_back(query);

    const float cell_size = _grid_spec.cell_size;

    vector<int> current, neighbor;
    bool neighbor_visit = 0;
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
                    // ((max_relative_dd < 0) || (fabs(neighbor_dist - query_dist) <= max_relative_dd*min(query_dist, neighbor_dist)))
                    if(!neighbor_visit && ((local_diff_range < 0) || (MinDistance(query, neighbor, cell_size)) <= local_diff_range*_jacobian_field.ForwardMaxEigenValue()))
                    {
                        // add neighbor
                        output.push_back(neighbor);
                        if(! _visit.Put(neighbor, 1)) throw Exception("SampledAnisotropicDiffGather::Find(): can't put visit");
                        active_list.push_back(neighbor);
                    }
                }
            }
    }

    // done
    return 1;
}
