/*
  Tromino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "Tromino.hpp"
#include "Exception.hpp"

Tromino::Tromino(void)
{
    // nothing to do
}

Tromino::Tromino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Polyomino(center, orientation, reflection)
{
    // nothing to do
}

Tromino::Tromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Polyomino(center, orientation, reflection, sample_offset)
{
    // nothing to do
}

Tromino::~Tromino(void)
{
    // nothing to do
}

Tromino * Tromino::Clone(void) const
{
    throw Exception("Tromino::Clone() should not be called");
    return 0;
}

int Tromino::NumSubdivisionRules(void) const
{
    return 0;
}

int Tromino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("Tromino::Subdivide() : should not be called");

    return 0;
}

void Tromino::GetCells(vector<Int2> & cells) const
{
    // shouldn't be called
    throw Exception("Tromino::GetCells(vector<Int2> & cells) should not be called");
}

    
