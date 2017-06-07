/*
  TTetromino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "TTetromino.hpp"
#include "Exception.hpp"

TTetromino::TTetromino(void)
{
    // nothing to do
}

TTetromino::TTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Tetromino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

TTetromino::TTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Tetromino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

TTetromino::~TTetromino(void)
{
    // nothing to do
}

TTetromino * TTetromino::Clone(void) const
{
    TTetromino * baby = new TTetromino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

void TTetromino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 4)
    {
        cells = vector<Int2>(4);
    }

    switch(_orientation)
    {
    case ON:
        cells[0].row = -1; cells[0].col = -1;
        cells[1].row = -1; cells[1].col =  0;
        cells[2].row = -1; cells[2].col =  1;
        cells[3].row =  0; cells[3].col =  0;
        break;

    case OE:
        cells[0].row = -1; cells[0].col =  0;
        cells[1].row =  0; cells[1].col = -1;
        cells[2].row =  0; cells[2].col =  0;
        cells[3].row =  1; cells[3].col =  0;
        break;

    case OS:
        cells[0].row = -1; cells[0].col = -1;
        cells[1].row =  0; cells[1].col = -2;
        cells[2].row =  0; cells[2].col = -1;
        cells[3].row =  0; cells[3].col =  0;
        break;

    case OW:
        cells[0].row = -2; cells[0].col = -1;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row = -1; cells[2].col =  0;
        cells[3].row =  0; cells[3].col = -1;
        break;
    }
}

    
