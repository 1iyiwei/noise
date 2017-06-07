/*
  ZTetromino.cpp

  Li-Yi Wei
  11/10/2007

*/

#include "ZTetromino.hpp"
#include "Exception.hpp"

ZTetromino::ZTetromino(void)
{
    // nothing to do
}

ZTetromino::ZTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Tetromino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

ZTetromino::ZTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Tetromino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

ZTetromino::~ZTetromino(void)
{
    // nothing to do
}

ZTetromino * ZTetromino::Clone(void) const
{
    ZTetromino * baby = new ZTetromino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

void ZTetromino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 4)
    {
        cells = vector<Int2>(4);
    }

    // center cell
    switch(_orientation)
    {
    case ON:
    case OS:
        cells[0].row = -1; cells[0].col = -1;
        cells[1].row = -1; cells[1].col =  0;
        cells[2].row =  0; cells[2].col = (_reflection == R1 ? 0 : -2);
        cells[3].row =  0; cells[3].col = (_reflection == R1 ? 1 : -1);
        break;

    case OE:
    case OW:
        cells[0].row = -1; cells[0].col = (_reflection == R1 ? 0 : -1);
        cells[1].row =  0; cells[1].col = -1;
        cells[2].row =  0; cells[2].col = 0;
        cells[3].row =  1; cells[3].col = (_reflection == R1 ? -1 : 0);
        break;
    }
}

    
