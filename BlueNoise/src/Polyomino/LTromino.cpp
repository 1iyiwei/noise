/*
  LTromino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "LTromino.hpp"
#include "Exception.hpp"

LTromino::LTromino(void)
{
    // nothing to do
}

LTromino::LTromino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Tromino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

LTromino::LTromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Tromino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

LTromino::~LTromino(void)
{
    // nothing to do
}

LTromino * LTromino::Clone(void) const
{
    LTromino * baby = new LTromino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

void LTromino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 3)
    {
        cells = vector<Int2>(3);
    }

    switch(_orientation)
    {
    case ON:
        cells[0].row = -1; cells[0].col = (_reflection == R1 ? -1 : 0);
        cells[1].row = 0; cells[1].col = -1;
        cells[2].row = 0; cells[2].col =  0;
        break;

    case OE:
        cells[0].row = (_reflection == R1 ? -1 : 0); cells[0].col = -1;
        cells[1].row = (_reflection == R1 ? -1 : 0); cells[1].col =  0;
        cells[2].row = (_reflection == R1 ? 0 : -1); cells[2].col = -1;
        break;

    case OS:
        cells[0].row = -1; cells[0].col = -1; 
        cells[1].row = -1; cells[1].col =  0;
        cells[2].row = 0; cells[2].col = (_reflection == R1 ? 0 : -1);
        break;

    case OW:
        cells[0].row = (_reflection == R1 ? 0 : -1); cells[0].col = -1;
        cells[1].row = (_reflection == R1 ? 0 : -1); cells[1].col =  0;
        cells[2].row = (_reflection == R1 ? -1 : 0); cells[2].col =  0;
        break;
    }
}

    
