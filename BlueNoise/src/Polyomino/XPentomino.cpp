/*
  XPentomino.cpp

  Li-Yi Wei
  11/10/2007

*/

#include "XPentomino.hpp"
#include "Exception.hpp"

XPentomino::XPentomino(void)
{
    // nothing to do
}

XPentomino::XPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    // nothing to do
    // all orientations and reflections are fine
}

XPentomino::XPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    // nothing to do
    // all orientations and reflections are fine
}

XPentomino::~XPentomino(void)
{
    // nothing to do
}

XPentomino * XPentomino::Clone(void) const
{
    XPentomino * baby = new XPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int XPentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int XPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("XPentomino::Subdivide(): not yet done");
    return 1;
}

void XPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    cells[0].row = -1; cells[0].col =  0;
    cells[1].row =  0; cells[1].col = -1;
    cells[2].row =  0; cells[2].col =  0;
    cells[3].row =  0; cells[3].col =  1;
    cells[4].row =  1; cells[4].col =  0;
}

    
