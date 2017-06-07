/*
  ITromino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "ITromino.hpp"
#include "Exception.hpp"

ITromino::ITromino(void)
{
    // nothing to do
}

ITromino::ITromino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Tromino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

ITromino::ITromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Tromino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

ITromino::~ITromino(void)
{
    // nothing to do
}

ITromino * ITromino::Clone(void) const
{
    ITromino * baby = new ITromino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

void ITromino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 3)
    {
        cells = vector<Int2>(3);
    }

    switch(_orientation)
    {
    case ON:
    case OS:
        cells[0].row = -1; cells[0].col = (_reflection == R1 ? 0 : -1);
        cells[1].row =  0; cells[1].col = (_reflection == R1 ? 0 : -1);
        cells[2].row =  1; cells[2].col = (_reflection == R1 ? 0 : -1);
        break;

    case OE:
    case OW:
        cells[0].row = (_reflection == R1 ? 0 : -1); cells[0].col = -1;
        cells[1].row = (_reflection == R1 ? 0 : -1); cells[1].col =  0;
        cells[2].row = (_reflection == R1 ? 0 : -1); cells[2].col =  1;
        break;
    }
}

    
