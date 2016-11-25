/*
  SampledDistanceField.cpp

  Li-Yi Wei
  07/15/2007

  08/26/2008
  modified for multi-class sampling

  09/14/2010
  modify back for single-class

*/

#include <math.h>

#include "Exception.hpp"
#include "SampledDistanceField.hpp"
#include "Math.hpp"

SampledDistanceField::SampledDistanceField(const Domain & domain, const SampledImportanceField & importance, const float average_min_distance) : DistanceField(domain), _importance(importance), _data(importance.Data()), _mean_distance(average_min_distance), _max_distance(0), _index(importance.Dimension())
{
    if(!CommonCheck(domain, DomainSize(importance.Data(), importance.CellSize())))
    {
        throw Exception("SampledDistanceField::SampledDistanceField(): illegal domain specification");
    }

    CommonConstruct(average_min_distance);
}

SampledDistanceField::SampledDistanceField(const Domain & domain, const Array<float> & importance, const float cell_size, const float average_min_distance) : DistanceField(domain), _importance(importance, cell_size), _data(importance), _mean_distance(average_min_distance), _max_distance(0), _index(importance.Dimension())
{
    if(!CommonCheck(domain, DomainSize(importance, cell_size)))
    {
        throw Exception("SampledDistanceField::SampledDistanceField(): illegal domain specification");
    }

    CommonConstruct(average_min_distance);
}

int SampledDistanceField::CommonCheck(const Domain & domain, const vector<float> & domain_size)
{
    vector<float> bbox(domain_size);
    if(! domain.BoundingBox(bbox))
    {
        return 0;
    }

    if(bbox.size() != domain_size.size())
    {
        return 0;
    }

    for(unsigned int i = 0; i < bbox.size(); i++)
    {
        if(bbox[i] > domain_size[i])
        {
            return 0;
        }
    }

    return 1;
}

void SampledDistanceField::CommonConstruct(const float average_min_distance)
{
    // convert importance into min_distance information
    vector<float> importance_data;
    vector<float> distance_data;
    vector<int> data_size = _importance.DataSize();

    {
        Array<float> & data = _data;

        if(! _importance.Data().Get(importance_data))
        {
            throw Exception("SampledDistanceField::SampledDistanceField() : cannot get importance data");
        }
    
        if(! data.Get(distance_data))
        {
            throw Exception("SampledDistanceField::SampledDistanceField() : cannot get distance data");
        }
    
        float average = 0;
        for(unsigned int i = 0; i < importance_data.size(); i++)
        {
            average += importance_data[i];
        }
        average /= importance_data.size();

        const int dimension = _importance.Dimension();
    
        const int max_num_samples = Math::ComputeMaxNumSamples(dimension, average_min_distance);

        assert(importance_data.size() == distance_data.size());
    
        for(unsigned int i = 0; i < distance_data.size(); i++)
        {
            const float local_max_num_samples = max_num_samples*importance_data[i]/average;
            distance_data[i] = Math::ComputeMaxMinDistance(dimension, local_max_num_samples);

            // compute max distance per class
            if(distance_data[i] > _max_distance)
            {
                _max_distance = distance_data[i];
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

float SampledDistanceField::Query(const Sample & sample_in) const
{
    // the idea way is to use multi-linear filtering
    // (extension of bilinear filtering)
    // but for now, use nearest neighbor first for simplicity and performance
    // can switch to multi-linear later

    {
        Sample sample(sample_in);

        assert(sample.coordinate.Dimension() == _index.size());
        
        if(!_domain.Adopt(sample_in, sample))
        {
            throw Exception("SampledDistanceField::Query(): cannot adopt sample");
        }

        const Array<float> & data = _data;

#if 1
        if(! LocateCell(sample, _importance.CellSize(), _index)) throw Exception("SampledDistanceField::Query(): cannot locate cell");
        for(unsigned int i = 0; i < _index.size(); i++)
        {
            if(_index[i] < 0) _index[i] = 0;
            if(_index[i] >= data.Size(i)) _index[i] = data.Size(i)-1;
        }
#else
        for(unsigned int i = 0; i < _index.size(); i++)
        {
            _index[i] = static_cast<int>(floor((sample.coordinate[i])/(_domain_size[i])*(data.Size(i)/*-1*/)/*+0.5*/));

            if(_index[i] < 0) _index[i] = 0;
            if(_index[i] >= data.Size(i)) _index[i] = data.Size(i)-1;
        }
#endif

        float answer = -1;
        if(!data.Get(_index, answer)) throw Exception("SampledDistanceField::Query() : cannot get data");

        return answer;
    }
}

float SampledDistanceField::Mean(void) const
{
    return _mean_distance;
}

const vector<int> & SampledDistanceField::DataSize(void) const
{
    return _importance.DataSize();
}

float SampledDistanceField::CellSize(void) const
{
    return _importance.CellSize();
}

float SampledDistanceField::Query(const vector<int> & index) const
{
    float value = -1;
    if(! _data.Get(index, value)) throw Exception("SampledDistanceField::Query() : cannot get data");

    return value;
}

float SampledDistanceField::Max(void) const
{
    return _max_distance;
}

vector<float> SampledDistanceField::DomainSize(const Array<float> & importance, const float cell_size)
{
    return SampledImportanceField::DomainSize(importance, cell_size);
}
