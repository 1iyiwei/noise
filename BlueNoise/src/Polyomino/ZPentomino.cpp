/*
  ZPentomino.cpp

  Li-Yi Wei
  11/10/2007

*/

#include "ZPentomino.hpp"
#include "Exception.hpp"

ZPentomino::ZPentomino(void)
{
    // nothing to do
}

ZPentomino::ZPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

ZPentomino::ZPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

ZPentomino::~ZPentomino(void)
{
    // nothing to do
}

ZPentomino * ZPentomino::Clone(void) const
{
    ZPentomino * baby = new ZPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int ZPentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int ZPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("ZPentomino::Subdivide() not yet done");
}

void ZPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    // center cell
    cells[0].row = 0; cells[0].col = 0;

    switch(_orientation)
    {
    case ON:
    case OS:
        cells[1].row = -1; cells[1].col = 0;
        cells[2].row =  1; cells[2].col = 0;
        cells[3].row = -1; cells[3].col = (_reflection == R1 ? -1 :  1);
        cells[4].row =  1; cells[4].col = (_reflection == R1 ?  1 : -1);
        break;

    case OE:
    case OW:
        cells[1].row = 0; cells[1].col = -1;
        cells[2].row = 0; cells[2].col =  1;
        cells[3].row = (_reflection == R1 ?  1 : -1); cells[3].col = -1;
        cells[4].row = (_reflection == R1 ? -1 :  1); cells[4].col =  1;
        break;
    }
}

    
