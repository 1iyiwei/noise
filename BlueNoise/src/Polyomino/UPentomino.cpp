/*
  UPentomino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "UPentomino.hpp"
#include "Exception.hpp"

UPentomino::UPentomino(void)
{
    // nothing to do
}

UPentomino::UPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

UPentomino::UPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

UPentomino::~UPentomino(void)
{
    // nothing to do
}

UPentomino * UPentomino::Clone(void) const
{
    UPentomino * baby = new UPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int UPentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int UPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("UPentomino::Subdivide(): not yet done");
    
    return 1;
}

void UPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    switch(_orientation)
    {
    case ON:
        cells[0].row = -1; cells[0].col = -1;
        cells[1].row =  0; cells[1].col = -1;
        cells[2].row =  0; cells[2].col =  0;
        cells[3].row =  0; cells[3].col =  1;
        cells[4].row = -1; cells[4].col =  1;
        break;

    case OE:
        cells[0].row = -1; cells[0].col =  0;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row =  0; cells[2].col = -1;
        cells[3].row =  1; cells[3].col = -1;
        cells[4].row =  1; cells[4].col =  0;
        break;

    case OS:
        cells[0].row =  0; cells[0].col = -2;
        cells[1].row = -1; cells[1].col = -2;
        cells[2].row = -1; cells[2].col = -1;
        cells[3].row = -1; cells[3].col =  0;
        cells[4].row =  0; cells[4].col =  0;
        break;

    case OW:
        cells[0].row = -1; cells[0].col = -1;
        cells[1].row = -1; cells[1].col =  0;
        cells[2].row =  0; cells[2].col =  0;
        cells[3].row =  1; cells[3].col =  0;
        cells[4].row =  1; cells[4].col = -1;
        break;
    }
}

    
