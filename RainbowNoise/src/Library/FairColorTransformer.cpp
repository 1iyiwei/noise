/*
  FairColorTransformer.cpp

  Li-Yi Wei
  09/26/2009

*/

#include <algorithm>
using namespace std;

#include "FairColorTransformer.hpp"
#include "Exception.hpp"

FairColorTransformer::FairColorTransformer(const int input_dimension, const vector<Vector> & output_basis) : ColorTransformer(input_dimension, output_basis), _output_weights(output_basis.size()), _output_indices(input_dimension), _input_sorted(input_dimension)
{
    // error checking and build _output_weights and _output_indices
    for(unsigned int i = 0; i < output_basis.size(); i++)
    {
        const Vector & output_vector = output_basis[i];

        if(output_vector.size() != input_dimension)
        {
            throw Exception("FairColorTransformer::ColorTransformer(): illegal output basis dimension");
        }

        float max_value = 0;
        for(unsigned int j = 0; j < output_vector.size(); j++)
        {
            if(output_vector[j] < 0)
            {
                throw Exception("FairColorTransformer::ColorTransformer(): negative output basis component");
            }

            if(output_vector[j] > 0)
            {
                _output_indices[j].push_back(i);

                if(max_value <= 0)
                {
                    max_value = output_vector[j];
                }
                else
                {
                    if(output_vector[j] != max_value)
                    {
                        throw Exception("FairColorTransformer::ColorTransformer(): illegal output basis component");
                    }
                }
            }
        }

        if(max_value <= 0)
        {
            throw Exception("FairColorTransformer::ColorTransformer(): zero output basis vector");
        }

        _output_weights[i] = max_value;
    }
}

FairColorTransformer::~FairColorTransformer(void)
{
    // nothing else to do
}

int FairColorTransformer::Transform(const Vector & input_color, Vector & output_weight) const
{
    if(input_color.size() != _input_sorted.size())
    {
        throw Exception("FairColorTransformer::Transform(): input_color.size() != _input_dimension");
        return 0;
    }

    for(unsigned int i = 0; i < _input_sorted.size(); i++)
    {
        _input_sorted[i].value = input_color[i];
        _input_sorted[i].index = i;
    }

    sort(_input_sorted.begin(), _input_sorted.end());

    output_weight = Vector(_output_basis.size(), -1.0);

    if(_input_sorted.size() != _output_indices.size())
    {
        throw Exception("FairColorTransformer::Transform(): _input_sorted.size() != _output_indices.size()");
    }

    for(unsigned int i_input = 0; i_input < _input_sorted.size(); i_input++)
    {
        float remaining_value = _input_sorted[i_input].value;
        const int i_output = _input_sorted[i_input].index;

        const vector<int> & output_index = _output_indices[i_output];

        int empty_count = 0;
        for(unsigned int j = 0; j < output_index.size(); j++)
        {
            if((output_index[j] < 0) || (output_index[j] >= output_weight.size()))
            {
                throw Exception("FairColorTransformer::Transform(): out of bound output_index[j]");
            }

            if(output_weight[output_index[j]] < 0)
            {
                empty_count += 1;
            }
            else
            {
                remaining_value -= output_weight[output_index[j]];
            }
        }

        if(empty_count <= 0)
        {
            throw Exception("FairColorTransformer::Transform(): empty_count <= 0"); 
        }

        if(remaining_value < 0)
        {
            throw Exception("FairColorTransformer::Transform(): remaining_value < 0"); 
        }

        const float equal_value = remaining_value/empty_count;
        for(unsigned int j = 0; j < output_index.size(); j++)
        {
            if(output_weight[output_index[j]] < 0)
            {
                // empty, fill it
                output_weight[output_index[j]] = equal_value;
            }
        }
    }

    // remember to divide
    if(output_weight.size() != _output_weights.size())
    {
        throw Exception("FairColorTransformer::Transform(): output_weight.size() != _output_weights.size()"); 
    }

    for(unsigned int i = 0; i < output_weight.size(); i++)
    {
        output_weight[i] /= _output_weights[i];
    }

    // done
    return 1;
}
