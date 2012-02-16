/*
  SampledRMatrixField.cpp

  Li-Yi Wei
  09/11/2008

*/

#include <math.h>

#include "SampledRMatrixField.hpp"
#include "SequentialCounter.hpp"
#include "Exception.hpp"

SampledRMatrixField::SampledRMatrixField(const SampledDistanceField & sampled_distance_field) : _domain_size(sampled_distance_field.Domain()), _data(sampled_distance_field.DataSize()), _index(sampled_distance_field.Dimension())
{
    // compute the r matrix for each sample point

    const int dimension = sampled_distance_field.Dimension();
    const int num_classes = sampled_distance_field.NumClasses();

    vector<int> index; _data.Size(index);
    vector<float> r_values(num_classes);
    Array<float> r_matrix(vector<int>(2, num_classes));

    vector<int> counter_min(index.size(), 0);
    vector<int> counter_max(index);
    for(unsigned int i = 0; i < counter_max.size(); i++)
    {
        counter_max[i] = index[i] - 1;
    }
    SequentialCounter counter(dimension, counter_min, counter_max);
    counter.Reset();

    do
    {
        counter.Get(index);

        for(unsigned int which_class = 0; which_class < r_values.size(); which_class++)
        {
            r_values[which_class] = sampled_distance_field.Query(which_class, index);
        }

        r_matrix = RMatrix::BuildRMatrix(dimension, num_classes, RMatrix::OPTIMAL, r_values);

        if(! _data.Put(index, r_matrix)) throw Exception("SampledRMatrixField::SampledRMatrixField() : cannot put back r_matrix");
    }
    while(counter.Next());

    // max distance
    _max_distance = 0;
    for(int which_class = 0; which_class < num_classes; which_class++)
    {
        const float max_distance_per_class = sampled_distance_field.MaxDistance(which_class);

        if(max_distance_per_class > _max_distance)
        {
            _max_distance = max_distance_per_class;
        }
    }
}

SampledRMatrixField::~SampledRMatrixField(void)
{
    // nothing to do
}

const Array<float> & SampledRMatrixField::Query(const Sample & sample) const
{
    // nearest sampling just like SampledDistanceField for now    
    {
        assert(sample.coordinate.Dimension() == _index.size());
    
        for(unsigned int i = 0; i < _index.size(); i++)
        {
            _index[i] = static_cast<int>(floor((sample.coordinate[i])/(_domain_size[i])*(_data.Size(i)/*-1*/)/*+0.5*/));

            if(_index[i] < 0) _index[i] = 0;
            if(_index[i] >= _data.Size(i)) _index[i] = _data.Size(i)-1;
        }

        if(!_data.Get(_index, _foo)) throw Exception("SampledDistanceField::Query() : cannot get data");

        return _foo;
    }
}
    
float SampledRMatrixField::MaxDistance(void) const
{
    return _max_distance;
}
