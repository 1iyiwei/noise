/*
  GHexomino.cpp

  Li-Yi Wei
  10/27/2007

*/

#include "GHexomino.hpp"
#include "GHexominoSubdivider.hpp"
#include "Exception.hpp"

GHexomino::GHexomino(void)
{
    // nothing to do
}

GHexomino::GHexomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Polyomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

GHexomino::GHexomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Polyomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

GHexomino::~GHexomino(void)
{
    // nothing to do
}

GHexomino * GHexomino::Clone(void) const
{
    GHexomino * baby = new GHexomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int GHexomino::NumSubdivisionRules(void) const
{
    return 1;
}

int GHexomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    vector<GHexomino *> g_hexomino_children;
    
    GHexominoSubdivider::Subdivide(this, g_hexomino_children);

    if(children.size() > 0)
    {
        throw Exception("GHexomino::Subdivide() : children.size() > 0");
    }
    else
    {
        for(unsigned int i = 0; i < g_hexomino_children.size(); i++)
        {
            children.push_back(g_hexomino_children[i]);
        }
    }

    return 1;
}

void GHexomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 6)
    {
        cells = vector<Int2>(6);
    }

    // center 4 cells
    cells[0].row = -1; cells[0].col = -1;
    cells[1].row = -1; cells[1].col =  0;
    cells[2].row =  0; cells[2].col = -1;
    cells[3].row =  0; cells[3].col =  0;

    switch(_orientation)
    {
    case ON:
        cells[4].row = -2; cells[4].col = (_reflection == R1 ? -1 :  0);
        cells[5].row = -1; cells[5].col = (_reflection == R1 ?  1 : -2);
        break;

    case OE:
        cells[4].col =  1; cells[4].row = (_reflection == R1 ? -1 :  0);
        cells[5].col =  0; cells[5].row = (_reflection == R1 ?  1 : -2);
        break;

    case OS:
        cells[4].row =  0; cells[4].col = (_reflection == R1 ? -2 :  1);
        cells[5].row =  1; cells[5].col = (_reflection == R1 ?  0 : -1);
        break;

    case OW:
        cells[4].col = -1; cells[4].row = (_reflection == R1 ? -2 :  1);
        cells[5].col = -2; cells[5].row = (_reflection == R1 ?  0 : -1);
        break;
    }
}
