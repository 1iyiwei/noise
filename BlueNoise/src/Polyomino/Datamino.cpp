/*
  Datamino.hpp

  Li-Yi Wei
  10/27/2007

*/

#include "Datamino.hpp"
#include "Exception.hpp"

Datamino::Datamino(void)
{
    // nothing to do
}

Datamino::Datamino(const Int2 & center, const vector<Int2> & cells) : Polyomino(center), _cells(cells)
{
    // nothing to do
}

Datamino::Datamino(const Int2 & center, const vector<Int2> & cells, const Flt2 & sample_offset) : Polyomino(center, sample_offset), _cells(cells)
{
    // nothing to do
}

Datamino::~Datamino(void)
{
    // nothing to do
}

Datamino * Datamino::Clone(void) const
{
    Datamino * baby = new Datamino(*this);

    baby->ClearNeighborList();
    
    return baby;
}

int Datamino::NumSubdivisionRules(void) const
{
    return 0;
}

int Datamino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("GHexomino::Subdivide() not yet done");
    return 0;
}

void Datamino::GetCells(vector<Int2> & cells) const
{
    cells = _cells;
}

#include "Exception.hpp"

void Datamino::ReflectRow(void)
{
    Polyomino::ReflectRow();

    throw Exception("Datamino::ReflectRow() not yet done");
}

void Datamino::ReflectCol(void)
{
    Polyomino::ReflectCol();

    throw Exception("Datamino::ReflectCol() not yet done");
}

void Datamino::Rotate90(void)
{
    Polyomino::Rotate90();

    throw Exception("Datamino::Rotate90() not yet done");
}
