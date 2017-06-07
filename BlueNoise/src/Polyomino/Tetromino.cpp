/*
  Tetromino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "Tetromino.hpp"
#include "Exception.hpp"

Tetromino::Tetromino(void)
{
    // nothing to do
}

Tetromino::Tetromino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Polyomino(center, orientation, reflection)
{
    // nothing to do
}

Tetromino::Tetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Polyomino(center, orientation, reflection, sample_offset)
{
    // nothing to do
}

Tetromino::~Tetromino(void)
{
    // nothing to do
}

Tetromino * Tetromino::Clone(void) const
{
    throw Exception("Tetromino::Clone() should not be called");
    return 0;
}

int Tetromino::NumSubdivisionRules(void) const
{
    throw Exception("Tetromino::NumSubdivisionRules() : should not be called");
    return 0;
}

int Tetromino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("Tetromino::Subdivide() : should not be called");

    return 0;
}

void Tetromino::GetCells(vector<Int2> & cells) const
{
    // shouldn't be called
    throw Exception("Tetromino::GetCells(vector<Int2> & cells) should not be called");
}

    
