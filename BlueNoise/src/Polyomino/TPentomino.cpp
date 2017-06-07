/*
  TPentomino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "TPentomino.hpp"
#include "Exception.hpp"

TPentomino::TPentomino(void)
{
    // nothing to do
}

TPentomino::TPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

TPentomino::TPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
}

TPentomino::~TPentomino(void)
{
    // nothing to do
}

TPentomino * TPentomino::Clone(void) const
{
    TPentomino * baby = new TPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int TPentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int TPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("TPentomino::Subdivide(): not yet done");
    
    return 1;
}

void TPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    switch(_orientation)
    {
    case ON:
        cells[0].row = -1; cells[0].col = -1;
        cells[1].row = -1; cells[1].col =  0;
        cells[2].row = -1; cells[2].col =  1;
        cells[3].row =  0; cells[3].col =  0;
        cells[4].row =  1; cells[4].col =  0;
        break;

    case OE:
        cells[0].row = -1; cells[0].col =  0;
        cells[1].row =  0; cells[1].col = -2;
        cells[2].row =  0; cells[2].col = -1;
        cells[3].row =  0; cells[3].col =  0;
        cells[4].row =  1; cells[4].col =  0;
        break;

    case OS:
        cells[0].row = -2; cells[0].col = -1;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row =  0; cells[2].col = -2;
        cells[3].row =  0; cells[3].col = -1;
        cells[4].row =  0; cells[4].col =  0;
        break;

    case OW:
        cells[0].row = -2; cells[0].col = -1;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row = -1; cells[2].col =  0;
        cells[3].row = -1; cells[3].col =  1;
        cells[4].row =  0; cells[4].col = -1;
        break;
    }
}

    
