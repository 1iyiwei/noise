/*
  SampledDistanceField.cpp

  Li-Yi Wei
  07/15/2007

*/

#include <math.h>

#include "Exception.hpp"
#include "SampledDistanceField.hpp"
#include "Math.hpp"

// #define _USE_GLOBAL_AVERAGE // should specify the per-class r values properly

SampledDistanceField::SampledDistanceField(const vector<float> & domain_size, const vector<float> & average_min_distances, const vector< Array<float> > & importances) : DistanceField(domain_size), _data(importances), _data_size(domain_size.size()), _max_distances(average_min_distances.size(), 0), _index(domain_size.size())
{
    if(average_min_distances.size() != importances.size())
    {
        throw Exception("SampledDistanceField::SampledDistanceField(): average_min_distances.size() != importances.size()");
    }

    // global average
    vector<float> importance_data;

#ifdef _USE_GLOBAL_AVERAGE
    float global_average = 0;
    for(unsigned int which_data = 0; which_data < _data.size(); which_data++)
    {
        const Array<float> & importance = importances[which_data];

        if(! importance.Get(importance_data))
        {
            throw Exception("SampledDistanceField::SampledDistanceField() : cannot get importance data");
        }

        float average = 0;
        for(unsigned int i = 0; i < importance_data.size(); i++)
        {
            average += importance_data[i];
        }
        average /= importance_data.size();

        global_average += average;
    }

    global_average /= _data.size();
#endif

    // convert importance into min_distance information
    vector<float> distance_data;
    vector<int> data_size(_data_size);
    for(unsigned int which_data = 0; which_data < _data.size(); which_data++)
    {
        const Array<float> & importance = importances[which_data];
        Array<float> & data = _data[which_data];
        const float average_min_distance = average_min_distances[which_data];

        if(which_data == 0)
        {
            importance.Size(_data_size);
        }
        else
        {
            importance.Size(data_size);

            // ensure consistent data size across all classes
            if(data_size.size() != _data_size.size())
            {
                throw Exception("SampledDistanceField::SampledDistanceField() : inconsistent data size");
            }

            for(unsigned int i = 0; i < data_size.size(); i++)
            {
                if(data_size[i] != _data_size[i])
                {
                    throw Exception("SampledDistanceField::SampledDistanceField() : inconsistent data size");
                }
            }
        }

        if(! importance.Get(importance_data))
        {
            throw Exception("SampledDistanceField::SampledDistanceField() : cannot get importance data");
        }
    
        if(! data.Get(distance_data))
        {
            throw Exception("SampledDistanceField::SampledDistanceField() : cannot get distance data");
        }
    
#ifdef _USE_GLOBAL_AVERAGE
        const float average = global_average;
#else
        float average = 0;
        for(unsigned int i = 0; i < importance_data.size(); i++)
        {
            average += importance_data[i];
        }
        average /= importance_data.size();
#endif

        const int dimension = domain_size.size();
    
        const int max_num_samples = Math::ComputeMaxNumSamples(dimension, average_min_distance);

        assert(importance_data.size() == distance_data.size());
    
        for(unsigned int i = 0; i < distance_data.size(); i++)
        {
            const float local_max_num_samples = max_num_samples*importance_data[i]/average;
            distance_data[i] = Math::ComputeMaxMinDistance(dimension, local_max_num_samples);

            // compute max distance per class
            if(distance_data[i] > _max_distances[which_data])
            {
                _max_distances[which_data] = distance_data[i];
            }
        }
    
        if(!data.Put(distance_data))
        {
            throw Exception("SampledDistanceField::SampledDistanceField() : cannot put distance data");
        }
    }
}

SampledDistanceField::~SampledDistanceField(void)
{
    // nothing to do
}

int SampledDistanceField::NumClasses(void) const
{
    return _data.size();
}

float SampledDistanceField::Query(const Sample & sample) const
{
    // the idea way is to use multi-linear filtering
    // (extension of bilinear filtering)
    // but for now, use nearest neighbor first for simplicity and performance
    // can switch to multi-linear later

    if((sample.id >= 0) && (sample.id < _data.size()))
    {
        assert(sample.coordinate.Dimension() == _index.size());
    
        const Array<float> & data = _data[sample.id];

        for(unsigned int i = 0; i < _index.size(); i++)
        {
            _index[i] = static_cast<int>(floor((sample.coordinate[i])/(_domain_size[i])*(data.Size(i)/*-1*/)/*+0.5*/));

            if(_index[i] < 0) _index[i] = 0;
            if(_index[i] >= data.Size(i)) _index[i] = data.Size(i)-1;
        }

        float answer = -1;
        if(!data.Get(_index, answer)) throw Exception("SampledDistanceField::Query() : cannot get data");

        return answer;
    }
    else
    {
        throw Exception("SampledDistanceField::Query() : illegal sample class id");
        return -1;
    }
}

SampledDistanceField::Boundary SampledDistanceField::BoundaryCondition(const int which_dimension) const
{
    // assumes uniform boundary handling across all dimensions
    // this can be expanded in the future to allow different 
    // boundary handling for different dimensions
    return BOUNDARY_TOROIDAL;
    // return BOUNDARY_NONE;
}

const vector<int> & SampledDistanceField::DataSize(void) const
{
    return _data_size;
}

float SampledDistanceField::Query(const int which_class, const vector<int> & index) const
{
    if((which_class < 0) || (which_class >= _data.size()))
    {
        throw Exception("SampledDistanceField::Query() : illegal class number");

        return -1;
    }
    else
    {
        float value = -1;
        if(! _data[which_class].Get(index, value)) throw Exception("SampledDistanceField::Query() : cannot get data");

        return value;
    }
}

float SampledDistanceField::MaxDistance(const int which_class) const
{
    if((which_class < 0) || (which_class >= _max_distances.size()))
    {
        throw Exception("SampledDistanceField::MaxDistance() : illegal input class number");
        return 0;
    }
    else
    {
        return _max_distances[which_class];
    }
}
