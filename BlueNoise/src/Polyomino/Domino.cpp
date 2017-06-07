/*
  Domino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "Domino.hpp"
#include "Exception.hpp"

Domino::Domino(void)
{
    // nothing to do
}

Domino::Domino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Polyomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

Domino::Domino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Polyomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

Domino::~Domino(void)
{
    // nothing to do
}

Domino * Domino::Clone(void) const
{
    Domino * baby = new Domino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int Domino::NumSubdivisionRules(void) const
{
    return 0;
}

int Domino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("Domino::Subdivide(): not yet done");
    return 1;
}

void Domino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 2)
    {
        cells = vector<Int2>(2);
    }

    switch(_orientation)
    {
    case ON:
    case OS:
        cells[0].row = -1; cells[0].col = 0;
        cells[1].row =  0; cells[1].col = 0;
        break;

    case OE:
    case OW:
        cells[0].row =  0; cells[0].col = -1;
        cells[1].row =  0; cells[1].col = 0;
        break;
    }
}

    
