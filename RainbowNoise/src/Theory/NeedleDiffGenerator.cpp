/*
  NeedleDiffGenerator.cpp

  Li-Yi Wei
  10/31/2008

*/

#include <algorithm>
using namespace std;

#include <math.h>

#include "NeedleDiffGenerator.hpp"
#include "Random.hpp"
#include "Exception.hpp"

NeedleDiffGenerator::NeedleDiffGenerator(const vector<float> & domain_size, const int needle_dimension, const float needle_length) : DiffGenerator(domain_size), _needle_dimension(max(needle_dimension, static_cast<int>(domain_size.size()))), _needle_length(needle_length), _needle_length_max_multiple(1), _coord(_needle_dimension)
{
    if(_needle_dimension <= 0) throw Exception("illegal needle dimension");
    if(_needle_length <= 0.0) throw Exception("illegal needle length");
}

NeedleDiffGenerator::NeedleDiffGenerator(const vector<float> & domain_size, const int needle_dimension, const float needle_length, const int needle_length_max_multiple) : DiffGenerator(domain_size), _needle_dimension(max(needle_dimension, static_cast<int>(domain_size.size()))), _needle_length(needle_length), _needle_length_max_multiple(needle_length_max_multiple), _coord(_needle_dimension)
{
    if(_needle_dimension <= 0) throw Exception("illegal needle dimension");
    if(_needle_length <= 0.0) throw Exception("illegal needle length");
}

NeedleDiffGenerator::~NeedleDiffGenerator(void)
{
    // nothing to do
}

int NeedleDiffGenerator::Produce(vector<Sample> & differences) const
{
    for(unsigned int i = 0; i < differences.size(); i++)
    {
        if(! Produce(differences[i]))
        {
            return 0;
        }
    }

    return 1;
}

int NeedleDiffGenerator::Produce(Sample & difference) const
{
    // generate a randomly oriented needle in a high dimension
    // via hypersphere rejection method
    const float needle_length_sqr = _needle_length * _needle_length;
    float coord_length_sqr = 0;

    do
    {
        coord_length_sqr = 0;

        for(int i = 0; i < _coord.Dimension(); i++)
        {
            _coord[i] = (2.0*Random::UniformRandom() - 1)*_needle_length;

            coord_length_sqr += _coord[i]*_coord[i];
        }
    }
    while((coord_length_sqr > needle_length_sqr) || (coord_length_sqr <= 0));

    // normalize to hypersphere boundary
    const float coord_length = sqrt(coord_length_sqr);
    const float length_scale = ceil(Random::UniformRandom()*_needle_length_max_multiple);

    for(int i = 0; i < _coord.Dimension(); i++)
    {
        _coord[i] = _coord[i]*_needle_length/coord_length*length_scale;
    }

    // projection to output
    difference.coordinate = Coordinate(_domain_size.size());
    for(int i = 0; i < difference.coordinate.Dimension(); i++)
    {
        difference.coordinate[i] = _coord[i];
    }

    return 1;
}
