/*
  VPentomino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "VPentomino.hpp"
#include "Exception.hpp"

VPentomino::VPentomino(void)
{
    // nothing to do
}

VPentomino::VPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Pentomino(center, orientation, reflection)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

VPentomino::VPentomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Pentomino(center, orientation, reflection, sample_offset)
{
    if(orientation == O0) throw Exception("illegal orientation");
    if(reflection == R0) throw Exception("illegal reflection");
}

VPentomino::~VPentomino(void)
{
    // nothing to do
}

VPentomino * VPentomino::Clone(void) const
{
    VPentomino * baby = new VPentomino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int VPentomino::NumSubdivisionRules(void) const
{
    return 0;
}

int VPentomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("VPentomino::Subdivide(): not yet done");
    
    return 1;
}

void VPentomino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 5)
    {
        cells = vector<Int2>(5);
    }

    OrientationReflection o_r = O0_R0;
    
    switch(_orientation)
    {
    case ON:
        o_r = (_reflection == R1 ? ON_R1 : OE_R2);
        break;

    case OE:
        o_r = (_reflection == R1 ? OE_R1 : OS_R2);
        break;

    case OS:
        o_r = (_reflection == R1 ? OS_R1 : OW_R2);
        break;

    case OW:
        o_r = (_reflection == R1 ? OW_R1 : ON_R2);
        break;
    }

    switch(o_r)
    {
    case ON_R1:
    case OE_R2:
        cells[0].row = -2; cells[0].col = -1;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row =  0; cells[2].col = -1;
        cells[3].row =  0; cells[3].col =  0;
        cells[4].row =  0; cells[4].col =  1;
        break;
        
    case OE_R1:
    case OS_R2:
        cells[0].row = -2; cells[0].col =  0;
        cells[1].row = -1; cells[1].col =  0;
        cells[2].row =  0; cells[2].col = -2;
        cells[3].row =  0; cells[3].col = -1;
        cells[4].row =  0; cells[4].col =  0;
        break;
        
    case OS_R1:
    case OW_R2:
        cells[0].row = -1; cells[0].col = -2;
        cells[1].row = -1; cells[1].col = -1;
        cells[2].row = -1; cells[2].col =  0;
        cells[3].row =  0; cells[3].col =  0;
        cells[4].row =  1; cells[4].col =  0;
        break;
        
    case OW_R1:
    case ON_R2:
        cells[0].row = -1; cells[0].col = -1;
        cells[1].row = -1; cells[1].col =  0;
        cells[2].row = -1; cells[2].col =  1;
        cells[3].row =  0; cells[3].col = -1;
        cells[4].row =  1; cells[4].col = -1;
        break;
    }
}

    
