/*
  HatDiffGenerator.cpp

  Li-Yi Wei
  10/30/2008

*/

#include <math.h>

#include "HatDiffGenerator.hpp"
#include "Random.hpp"

HatDiffGenerator::HatDiffGenerator(const vector<float> & domain_size) : DiffGenerator(domain_size), _mine_field(_empty_mine_field), _coord0(domain_size.size()), _coord1(_coord0)
{
    // nothing to do
}

HatDiffGenerator::HatDiffGenerator(const vector<float> & domain_size, const MineField & mine_field) : DiffGenerator(domain_size), _mine_field(mine_field), _coord0(domain_size.size()), _coord1(_coord0)
{
    // nothing to do
}

HatDiffGenerator::~HatDiffGenerator(void)
{
    // nothing to do
}

int HatDiffGenerator::Produce(vector<Sample> & differences) const
{
    for(unsigned int i = 0; i < differences.size(); i++)
    {
        if(! Produce(differences[i]))
        {
            i--;
        }
    }

    return 1;
}

int HatDiffGenerator::Produce(Sample & difference) const
{
    _coord0.id = _coord1.id = 0; // hack, assuming single class for now

    for(int i = 0; i < _coord0.coordinate.Dimension(); i++)
    {
        _coord0.coordinate[i] = Random::UniformRandom();
        _coord1.coordinate[i] = Random::UniformRandom();
    }

    if(!_mine_field.Alive(_coord0) || !_mine_field.Alive(_coord1))
    {
        return 0;
    }

    difference.coordinate = _coord0.coordinate;
    for(int i = 0; i < difference.coordinate.Dimension(); i++)
    {
        difference.coordinate[i] = (_coord0.coordinate[i] - _coord1.coordinate[i])*_domain_size[i];
    }

    return 1;
}
