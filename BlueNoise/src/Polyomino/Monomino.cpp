/*
  Monomino.cpp

  Li-Yi Wei

  10/21/2007

*/

#include "Monomino.hpp"
#include "Exception.hpp"

#include <math.h>

Monomino::Monomino(void)
{
    // nothing to do
}

Monomino::Monomino(const Int2 & center) : Polyomino(center)
{
    // nothing to do
}

Monomino::Monomino(const Int2 & center, const Flt2 & sample_offset) : Polyomino(center, sample_offset)
{
    // nothing to do
}

Monomino::~Monomino(void)
{
    // nothing to do
}

Monomino * Monomino::Clone(void) const
{
    Monomino * baby = new Monomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int Monomino::NumSubdivisionRules(void) const
{
    return 0;
}

int Monomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("Monomino::Subdivide() not yet done");
    return 0;
}

void Monomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 1)
    {
        cells = vector<Int2>(1);
    }

    cells[0].row = 0;
    cells[0].col = 0;
}

int Monomino::IsInside(const Flt2 & sample_offset) const
{
    return ((floor(sample_offset.row) == 0) && (floor(sample_offset.col) == 0));
}
