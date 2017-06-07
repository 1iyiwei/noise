/*
  Polyomino.cpp

  Li-Yi Wei
  10/21/2007

*/

#include "Polyomino.hpp"
#include "Exception.hpp"
#include "Random.hpp"

#include <math.h>

Polyomino::Polyomino(void)
{
    // nothing to do
}

Polyomino::Polyomino(const Int2 & center) : _center(center), _orientation(O0), _reflection(R0)
{
    // nothing to do
}

Polyomino::Polyomino(const Int2 & center, const Flt2 & sample_offset) : _center(center), _orientation(O0), _reflection(R0), _sample_offset(sample_offset)
{
    // nothing to do
}

Polyomino::Polyomino(const Int2 & center, const Orientation orientation, const Reflection reflection) : _center(center), _orientation(orientation), _reflection(reflection)
{
    // nothing to do
}

Polyomino::Polyomino(const Int2 & center, const Orientation orientation, const Reflection reflection, const Flt2 & sample_offset) : _center(center), _orientation(orientation), _reflection(reflection), _sample_offset(sample_offset)
{
    // nothing to do
}

Polyomino::~Polyomino(void)
{
    // nothing to do
}

Polyomino * Polyomino::Clone(void) const
{
    throw Exception("Polyomino::Clone() should not be called");
    return 0;
}

int Polyomino::NumSubdivisionRules(void) const
{
    return 0;
}

int Polyomino::Subdivide(vector<Polyomino *> & children, const int which_rule) const
{
    throw Exception("Polyomino::Subdivide() should not be called");
    return 0;
}

void Polyomino::GetCells(vector<Int2> & cells) const
{
    // shouldn't be called
    throw Exception("Polyomino::GetCells(vector<Int2> & cells) should not be called");
}

int Polyomino::IsInside(const Flt2 & sample_offset) const
{
    vector<Int2> cells;

    GetCells(cells);

    const Int2 sample_home(static_cast<int>(floor(sample_offset.row)), static_cast<int>(floor(sample_offset.col)));

    for(unsigned int i = 0; i < cells.size(); i++)
    {
        if((cells[i].row == sample_home.row) && (cells[i].col == sample_home.col))
        {
            return 1;
        }
    }

    return 0;
}

void Polyomino::GetCenter(Int2 & center) const
{
    center = _center;
}

void Polyomino::SetCenter(const Int2 & center)
{
    _center = center;
}

void Polyomino::GetOrientation(Orientation & orientation) const
{
    orientation = _orientation;
}

void Polyomino::SetOrientation(const Orientation & orientation)
{
    _orientation = orientation;
}
    
void Polyomino::GetReflection(Reflection & reflection) const
{
    reflection = _reflection;
}

void Polyomino::SetReflection(const Reflection & reflection)
{
    _reflection = reflection;
}

void Polyomino::GetOffset(Flt2 & sample_offset) const
{
    sample_offset = _sample_offset;
}

void Polyomino::SetOffset(const Flt2 & sample_offset)
{
    _sample_offset = sample_offset;
}

void Polyomino::ReflectRow(void)
{
    // center
    _center.row *= -1;
    
    // orientation
    if(_orientation == ON)
    {
        _orientation = OS;
    }
    else if(_orientation == OS)
    {
        _orientation = ON;
    }

    // reflection
    if(_reflection == R1)
    {
        _reflection = R2;
    }
    else if(_reflection == R2)
    {
        _reflection = R1;
    }
    
    // offset
    _sample_offset.row *= -1;
}

void Polyomino::ReflectCol(void)
{
    // center
    _center.col *= -1;
    
    // orientation
    if(_orientation == OE)
    {
        _orientation = OW;
    }
    else if(_orientation == OW)
    {
        _orientation = OE;
    }

    // reflection
    if(_reflection == R1)
    {
        _reflection = R2;
    }
    else if(_reflection == R2)
    {
        _reflection = R1;
    }
    
    // offset
    _sample_offset.col *= -1;
}

void Polyomino::Rotate90(void)
{
    // center
    _center = Int2(-_center.col, _center.row);
    
    // orientation
    if(_orientation == ON)
    {
        _orientation = OW;
    }
    else if(_orientation == OW)
    {
        _orientation = OS;
    }
    else if(_orientation == OS)
    {
        _orientation = OE;
    }
    else if(_orientation == OE)
    {
        _orientation = ON;
    }
    
    // reflection
    // nothing to do
    
    // offset
    _sample_offset = Flt2(-_sample_offset.col, _sample_offset.row);
}

void Polyomino::Rotate180(void)
{
    Rotate90(); Rotate90();
}

void Polyomino::Rotate270(void)
{
    Rotate90(); Rotate90(); Rotate90();
}

void Polyomino::ClearNeighborList(void)
{
    _neighbors.clear();
}

void Polyomino::AddNeighbor(const Neighbor & neighbor)
{
    _neighbors.push_back(neighbor);
}

int Polyomino::NumNeighbors(void) const
{
    return _neighbors.size();
}

Polyomino::Neighbor Polyomino::GetNeighbor(const int which) const
{
    if((which >= 0) && (static_cast<unsigned int>(which) < _neighbors.size()))
    {
        return _neighbors[which];
    }
    else
    {
        return Neighbor(0, Int2(0, 0));
    }
}

Polyomino::Flt2 Polyomino::RandomOffset(void) const
{
     vector<Int2> cells;

     GetCells(cells);

     Flt2 answer(0, 0);
     
     if(cells.size() > 0)
     {
         int which_cell = static_cast<int>(floor(Random::UniformRandom()*(cells.size()-1)));
         if(which_cell >= cells.size()) which_cell = cells.size()-1;
         if(which_cell < 0) which_cell = 0;

         answer.row = cells[which_cell].row + Random::UniformRandom();
         answer.col = cells[which_cell].col + Random::UniformRandom();
     }

     return answer;
}

Polyomino::Flt2 Polyomino::PullInOffset(const Flt2 & input) const
{
    vector<Int2> cells;

    GetCells(cells);

    const Int2 input_home(static_cast<int>(floor(input.row)), static_cast<int>(floor(input.col)));

    Int2 inside(0, 0);
    for(unsigned int i = 0; i < cells.size(); i++)
    {
        if(cells[i].row == input_home.row) inside.row = 1;
        if(cells[i].col == input_home.col) inside.col = 1;
    }

    if(inside.row && inside.col)
    {
        return input;
    }
    else
    {
        Flt2 min_correction(inside.row ? 0 : 1.0/sin(0.0), inside.col ? 0 : 1.0/sin(0.0));

        for(unsigned int i = 0; i < cells.size(); i++)
        {
            for(int row_o = 0; row_o <= 1; row_o++)
                for(int col_o = 0; col_o <= 1; col_o++)
                {
                    const Int2 current_vertex(cells[i].row + row_o, cells[i].col + col_o);
                    const Flt2 current_distance(input.row - current_vertex.row, input.col - current_vertex.col);

                    if(fabs(min_correction.row) > fabs(current_distance.row))
                    {
                        min_correction.row = current_distance.row;
                    }

                    if(fabs(min_correction.col) > fabs(current_distance.col))
                    {
                        min_correction.col = current_distance.col;
                    }
                }
        }

        return Flt2(input.row - min_correction.row, input.col - min_correction.col);
    }
}
    
