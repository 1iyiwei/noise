/*
  HatDiffGenerator.hpp

  hat diff generator for white noise
  (whose location difference has a hat shaped pdf)

  Li-Yi Wei
  10/30/2008

*/

#ifndef _HAT_DIFF_GENERATOR_HPP
#define _HAT_DIFF_GENERATOR_HPP

#include "DiffGenerator.hpp"
#include "EmptyMineField.hpp"

class HatDiffGenerator : public DiffGenerator
{
public:
    HatDiffGenerator(const vector<float> & domain_size);
    HatDiffGenerator(const vector<float> & domain_size, const MineField & mine_field);
    ~HatDiffGenerator(void);

    int Produce(vector<Sample> & differences) const;

protected:
    int Produce(Sample & difference) const;

protected:
    const EmptyMineField _empty_mine_field;
    const MineField & _mine_field;

    mutable Sample _coord0, _coord1;
};

#endif

 
