/*
  IPentomino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "IPentomino.hpp"
#include "Exception.hpp"

IPentomino::IPentomino(void)
{
    // nothing to do
}

IPentomino::IPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

IPentomino::IPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

IPentomino::~IPentomino(void)
{
    // nothing to do
}

IPentomino * IPentomino::Clone(void) const
{
    IPentomino * baby = new IPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int IPentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int IPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("IPentomino::Subdivide(): not yet done");
    return 1;
}

void IPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    switch(_orientation)
    {
    case ON:
    case OS:
        cells[0].row = -2; cells[0].col = (_reflection == R1 ? 0 : -1);
        cells[1].row = -1; cells[1].col = (_reflection == R1 ? 0 : -1);
        cells[2].row =  0; cells[2].col = (_reflection == R1 ? 0 : -1);
        cells[3].row =  1; cells[3].col = (_reflection == R1 ? 0 : -1);
        cells[4].row =  2; cells[4].col = (_reflection == R1 ? 0 : -1);
        break;

    case OE:
    case OW:
        cells[0].row = (_reflection == R1 ? 0 : -1); cells[0].col = -2;
        cells[1].row = (_reflection == R1 ? 0 : -1); cells[1].col = -1;
        cells[2].row = (_reflection == R1 ? 0 : -1); cells[2].col =  0;
        cells[3].row = (_reflection == R1 ? 0 : -1); cells[3].col =  1;
        cells[4].row = (_reflection == R1 ? 0 : -1); cells[4].col =  2;
        break;
    }
}

    
