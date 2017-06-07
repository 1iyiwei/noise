/*
  YPentomino.cpp

  Li-Yi Wei
  11/10/2007

*/

#include "YPentomino.hpp"
#include "Exception.hpp"

YPentomino::YPentomino(void)
{
    // nothing to do
}

YPentomino::YPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

YPentomino::YPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

YPentomino::~YPentomino(void)
{
    // nothing to do
}

YPentomino * YPentomino::Clone(void) const
{
    YPentomino * baby = new YPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int YPentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int YPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("YPentomino::Subdivide(): not yet done");

    return 1;
}

void YPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    switch(_orientation)
    {
    case ON:
        cells[0].row = -2; cells[0].col = 0;
        cells[1].row = -1; cells[1].col = 0;
        cells[2].row =  0; cells[2].col = 0;
        cells[3].row =  1; cells[3].col = 0;
        cells[4].row = -1; cells[4].col = (_reflection == R1 ? -1 : 1);
        break;

    case OE:
        cells[0].row = 0; cells[0].col = -2;
        cells[1].row = 0; cells[1].col = -1;
        cells[2].row = 0; cells[2].col =  0;
        cells[3].row = 0; cells[3].col =  1;
        cells[4].row = (_reflection == R1 ? -1 : 1); cells[4].col = 0;
        break;

    case OS:
        cells[0].row = -2; cells[0].col = -1;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row =  0; cells[2].col = -1;
        cells[3].row =  1; cells[3].col = -1;
        cells[4].row =  0; cells[4].col = (_reflection == R1 ? 0 : -2);
        break;

    case OW:
        cells[0].row = -1; cells[0].col = -2;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row = -1; cells[2].col =  0;
        cells[3].row = -1; cells[3].col =  1;
        cells[4].row = (_reflection == R1 ? 0 : -2); cells[4].col = -1;
        break;
    }
}

    
