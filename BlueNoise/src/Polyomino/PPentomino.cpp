/*
  PPentomino.cpp

  Li-Yi Wei
  10/25/2007

*/

#include "PPentomino.hpp"
#include "PPentominoSubdivider.hpp"
#include "Exception.hpp"

PPentomino::PPentomino(void)
{
    // nothing to do
}

PPentomino::PPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

PPentomino::PPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

PPentomino::~PPentomino(void)
{
    // nothing to do
}

PPentomino * PPentomino::Clone(void) const
{
    PPentomino * baby = new PPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int PPentomino::NumSubdivisionRules(void) const
{
    return 2;
}

int PPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    vector<PPentomino *> p_pentomino_children;
    
    PPentominoSubdivider::Subdivide(this, p_pentomino_children, which_rule);

    if(children.size() > 0)
    {
        throw Exception("PPentomino::Subdivide() : children.size() > 0");
    }
    else
    {
        for(unsigned int i = 0; i < p_pentomino_children.size(); i++)
        {
            children.push_back(p_pentomino_children[i]);
        }
    }

    return 1;
}

void PPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    cells[0].row = -1; cells[0].col = -1;
    cells[1].row = -1; cells[1].col =  0;
    cells[2].row =  0; cells[2].col = -1;
    cells[3].row =  0; cells[3].col =  0;

    switch(_orientation)
    {
    case ON:
        cells[4].row =  1; cells[4].col = (_reflection == R1 ? -1 : 0);
        break;

    case OE:
        cells[4].col = -2; cells[4].row = (_reflection == R1 ? -1 : 0);
        break;

    case OS:
        cells[4].row = -2; cells[4].col = (_reflection == R1 ? 0 : -1);
        break;

    case OW:
        cells[4].col =  1; cells[4].row = (_reflection == R1 ? 0 : -1);
        break;
    }
}

    
