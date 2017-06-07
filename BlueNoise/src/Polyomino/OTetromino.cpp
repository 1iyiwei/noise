/*
  OTetromino.cpp

  Li-Yi Wei
  11/11/2007

*/

#include "OTetromino.hpp"
#include "Exception.hpp"

OTetromino::OTetromino(void)
{
    // nothing to do
}

OTetromino::OTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection) : Tetromino(center, orientation, reflection)
{
    // nothing to do
    // all orientations and reflections are fine
}

OTetromino::OTetromino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : Tetromino(center, orientation, reflection, sample_offset)
{
    // nothing to do
    // all orientations and reflections are fine
}

OTetromino::~OTetromino(void)
{
    // nothing to do
}

OTetromino * OTetromino::Clone(void) const
{
    OTetromino * baby = new OTetromino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

void OTetromino::GetCells(vector<Int2> & cells) const
{
    if(cells.size() != 4)
    {
        cells = vector<Int2>(4);
    }

    cells[0].row = -1; cells[0].col = -1;
    cells[1].row = -1; cells[1].col =  0;
    cells[2].row =  0; cells[2].col = -1;
    cells[3].row =  0; cells[3].col =  0;
}

    
