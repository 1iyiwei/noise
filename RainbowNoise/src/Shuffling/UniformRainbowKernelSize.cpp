/*
  UniformRainbowKernelSize.cpp

  Li-Yi Wei
  08/29/2009

*/

#include "UniformRainbowKernelSize.hpp"
#include "Exception.hpp"

UniformRainbowKernelSize::UniformRainbowKernelSize(const vector<float> & domain_size, const RMatrix::Method method, const vector<int> & num_samples_per_class) : UniformKernelSize(domain_size, num_samples_per_class), _r_matrix(RMatrix::BuildRMatrix(domain_size, method, num_samples_per_class)), _index(2)
{
    // need to divide the entries of _r_matrix by 2
    Array<float> & r_matrix = const_cast< Array<float> & >(_r_matrix);
    {
        vector<float> elements;
        r_matrix.Get(elements);
        for(unsigned int i = 0; i < elements.size(); i++)
        {
            elements[i] *= 0.5;
        }
        r_matrix.Put(elements);
    }
}

float UniformRainbowKernelSize::GetSize(const Sample & sample, const vector<int> & source_classes) const
{
    int which_dim = 0;
    for(unsigned int i = 0; i < source_classes.size(); i++)
    {
        if(source_classes[i] > 0)
        {
            if(which_dim >= _index.size())
            {
                throw Exception("UniformRainbowKernelSize::GetSize(): no more than 2 classes can be active!");
                return 0;
            }
            else
            {
                for(unsigned int j = which_dim; j < _index.size(); j++)
                {
                    _index[j] = i;
                }

                which_dim++;
            }
        }
    }

    float value = 0;
    if(! _r_matrix.Get(_index, value))
    {
        throw Exception("UniformRainbowKernelSize::GetSize(): cannot get value");
        return 0;
    }

    return value;
}

float UniformRainbowKernelSize::GetSize(const Sample & sample, const int class_id0, const int class_id1) const
{
    _index[0] = class_id0;
    _index[1] = class_id1;

    // in case one entry is negative for class_id = -1, i.e. point
    for(unsigned int i = 0; i < _index.size(); i++)
    {
        if(_index[i] < 0)
        {
            _index[i] = _index[(i+1)%_index.size()];
        }
    }

    float value = 0;
    if(! _r_matrix.Get(_index, value))
    {
        throw Exception("UniformRainbowKernelSize::GetSize(): cannot get value");
        return 0;
    }

    return value;
}
