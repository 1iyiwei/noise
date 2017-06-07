/*
  LTetromino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "LTetromino.hpp"
#include "Exception.hpp"

LTetromino::LTetromino(void)
{
    // nothing to do
}

LTetromino::LTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Tetromino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

LTetromino::LTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Tetromino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

LTetromino::~LTetromino(void)
{
    // nothing to do
}

LTetromino * LTetromino::Clone(void) const
{
    LTetromino * baby = new LTetromino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

void LTetromino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 4)
    {
        cells = vector<Int2>(4);
    }

    switch(_orientation)
    {
    case ON:
        cells[0].row = -2; cells[0].col = (_reflection == R1 ? -1 : 0);
        cells[1].row = -1; cells[1].col = (_reflection == R1 ? -1 : 0);
        cells[2].row =  0; cells[2].col = (_reflection == R1 ? -1 : 0);
        cells[3].row =  0; cells[3].col = (_reflection == R1 ?  0 : -1);
        break;

    case OE:
        cells[0].row = (_reflection == R1 ? -1 : 0); cells[0].col =  1;
        cells[1].row = (_reflection == R1 ? -1 : 0); cells[1].col =  0;
        cells[2].row = (_reflection == R1 ? -1 : 0); cells[2].col = -1;
        cells[3].row = (_reflection == R1 ? 0 : -1); cells[3].col = -1;
        break;

    case OS:
        cells[0].row =  1; cells[0].col = (_reflection == R1 ? 0 : -1);
        cells[1].row =  0; cells[1].col = (_reflection == R1 ? 0 : -1);
        cells[2].row = -1; cells[2].col = (_reflection == R1 ? 0 : -1);
        cells[3].row = -1; cells[3].col = (_reflection == R1 ? -1 : 0);
        break;

    case OW:
        cells[0].row = (_reflection == R1 ? 0 : -1); cells[0].col = -2;
        cells[1].row = (_reflection == R1 ? 0 : -1); cells[1].col = -1;
        cells[2].row = (_reflection == R1 ? 0 : -1); cells[2].col =  0;
        cells[3].row = (_reflection == R1 ? -1 : 0); cells[3].col =  0;
        break;
    }
}

    
