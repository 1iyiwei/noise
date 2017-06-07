/*
  NPentomino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "NPentomino.hpp"
#include "Exception.hpp"

NPentomino::NPentomino(void)
{
    // nothing to do
}

NPentomino::NPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

NPentomino::NPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

NPentomino::~NPentomino(void)
{
    // nothing to do
}

NPentomino * NPentomino::Clone(void) const
{
    NPentomino * baby = new NPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int NPentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int NPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("NPentomino::Subdivide(): not yet done");
    return 1;
}

void NPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    switch(_orientation)
    {
    case ON:
        cells[0].row = -2; cells[0].col = (_reflection == R1 ?  0 : -1);
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row = -1; cells[2].col =  0;
        cells[3].row =  0; cells[3].col = (_reflection == R1 ? -1 :  0);
        cells[4].row =  1; cells[4].col = (_reflection == R1 ? -1 :  0);
        break;

    case OE:
        cells[0].row = (_reflection == R1 ? -1 :  0); cells[0].col = -2;
        cells[1].row = (_reflection == R1 ? -1 :  0); cells[1].col = -1;
        cells[2].row = -1; cells[2].col = 0;
        cells[3].row =  0; cells[3].col = 0;
        cells[4].row = (_reflection == R1 ?  0 : -1); cells[4].col =  1;
        break;

    case OS:
        cells[0].row = -2; cells[0].col = (_reflection == R1 ?  0 : -1);
        cells[1].row = -1; cells[1].col = (_reflection == R1 ?  0 : -1);
        cells[2].row = -1; cells[2].col =  0;
        cells[3].row =  0; cells[3].col = -1;
        cells[4].row =  1; cells[4].col = (_reflection == R1 ? -1 :  0);
        break;

    case OW:
        cells[0].row = (_reflection == R1 ? -1 :  0); cells[0].col = -2;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row =  0; cells[2].col = -1;
        cells[3].row = (_reflection == R1 ? 0 : -1); cells[3].col = 0;
        cells[4].row = (_reflection == R1 ? 0 : -1); cells[4].col = 1;
        break;
    }
}

    
