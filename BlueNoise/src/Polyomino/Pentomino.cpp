/*
  Pentomino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "Pentomino.hpp"
#include "Exception.hpp"

Pentomino::Pentomino(void)
{
    // nothing to do
}

Pentomino::Pentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Polyomino(center, orientation, reflection)
{
    // nothing to do
}

Pentomino::Pentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Polyomino(center, orientation, reflection, sample_offset)
{
    // nothing to do
}

Pentomino::~Pentomino(void)
{
    // nothing to do
}

Pentomino * Pentomino::Clone(void) const
{
    throw Exception("Pentomino::Clone() should not be called");
    return 0;
}

int Pentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int Pentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("Pentomino::Subdivide() : should not be called");

    return 0;
}

void Pentomino::GetCells(vector<Int2> & cells) const
{
    // shouldn't be called
    throw Exception("Pentomino::GetCells(vector<Int2> & cells) should not be called");
}

    
