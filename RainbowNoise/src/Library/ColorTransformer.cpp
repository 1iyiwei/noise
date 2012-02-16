/*
  ColorTransformer.cpp

  Li-Yi Wei
  09/26/2009

*/

#include "ColorTransformer.hpp"
#include "Exception.hpp"

ColorTransformer::ColorTransformer(const int input_dimension, const vector<Vector> & output_basis) : _input_dimension(input_dimension), _output_basis(output_basis)
{
    // consistency check
    if((input_dimension <= 0) || (output_basis.size() < input_dimension))
    {
        throw Exception("ColorTransformer::ColorTransformer(): illegal input dimension");
    }

    for(unsigned int i = 0; i < output_basis.size(); i++)
    {
        if(output_basis[i].size() != input_dimension)
        {
            throw Exception("ColorTransformer::ColorTransformer(): illegal output basis dimension");
        }
    }

    // make sure sum of each input component > 0
    for(int i = 0; i < input_dimension; i++)
    {
        float sum = 0;
        for(unsigned int j = 0; j < output_basis.size(); j++)
        {
            sum += output_basis[j][i];
        }

        if(sum <= 0)
        {
            throw Exception("ColorTransformer::ColorTransformer(): sum of input component must be > 0");
        }
    }
}

ColorTransformer::~ColorTransformer(void)
{
    // nothing else to do
}

int ColorTransformer::Transform(const Vector & input_color, Vector & output_weight) const
{
    throw Exception("ColorTransformer::Transform(): shouldn't be called");
    return 0;
}
