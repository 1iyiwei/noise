/*
  SampledDistanceField.cpp

  Li-Yi Wei
  07/15/2007

*/

#include <math.h>

#include "Exception.hpp"
#include "SampledDistanceField.hpp"
#include "Math.hpp"

SampledDistanceField::SampledDistanceField(const vector<float> & range_min, const vector<float> & range_max, const float average_min_distance, const Array<float> & importance) : DistanceField(range_min, range_max), _data(importance)
{
    // convert importance into min_distance information

    vector<float> importance_data;
    vector<float> distance_data;
    if(! importance.Get(importance_data))
    {
        throw Exception("SampledDistanceField::SampledDistanceField() : cannot get importance data");
    }
    if(! _data.Get(distance_data))
    {
        throw Exception("SampledDistanceField::SampledDistanceField() : cannot get distance data");
    }
    
    float average = 0;
    for(unsigned int i = 0; i < importance_data.size(); i++)
    {
        average += importance_data[i];
    }
    average /= importance_data.size();

    const int dimension = range_min.size();
    
    const int max_num_samples = Math::ComputeMaxNumSamples(dimension, average_min_distance);

    assert(importance_data.size() == distance_data.size());
    
    for(unsigned int i = 0; i < distance_data.size(); i++)
    {
        const int local_max_num_samples = static_cast<int>(floor(max_num_samples*importance_data[i]/average+0.5));
        distance_data[i] = Math::ComputeMaxMinDistance(dimension, local_max_num_samples);
    }
    
    if(!_data.Put(distance_data))
    {
        throw Exception("SampledDistanceField::SampledDistanceField() : cannot put distance data");
    }
}

SampledDistanceField::~SampledDistanceField(void)
{
    // nothing to do
}

float SampledDistanceField::Query(const Coordinate & sample) const
{
    // the idea way is to use multi-linear filtering
    // (extension of bilinear filtering)
    // but for now, use nearest neighbor first for simplicity and performance
    // can switch to multi-linear later

    vector<int> index(_range_min.size());

    assert(sample.Dimension() == index.size());
    
    for(unsigned int i = 0; i < index.size(); i++)
    {
        index[i] = static_cast<int>(floor((sample[i]-_range_min[i])/(_range_max[i]-_range_min[i])*(_data.Size(i)/*-1*/)/*+0.5*/));

        if(index[i] < 0) index[i] = 0;
        if(index[i] >= _data.Size(i)) index[i] = _data.Size(i)-1;
    }

    float answer = -1;
    _data.Get(index, answer);

    return answer;
}

float SampledDistanceField::QueryMax(const Coordinate & region_min, const Coordinate & region_max) const
{
    throw Exception("SampledDistanceField::QueryMax(0) not yet done but not needed anyway");
    // not yet done, but this function is not needed anyway
    return -1;
}

SampledDistanceField::Boundary SampledDistanceField::BoundaryCondition(const int which_dimension) const
{
    // assumes uniform boundary handling across all dimensions
    // this can be expanded in the future to allow different 
    // boundary handling for different dimensions
    return BOUNDARY_TOROIDAL;
    // return BOUNDARY_NONE;
}
