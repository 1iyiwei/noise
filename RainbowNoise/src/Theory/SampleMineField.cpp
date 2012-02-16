/*
  SampleMineField.cpp

  Li-Yi Wei
  11/09/2008

*/

#include "SampleMineField.hpp"

SampleMineField::SampleMineField(const Grid & grid, const ConflictChecker & conflict_checker) : _grid(grid), _conflict_checker(conflict_checker)
{
    // nothing to do
}

SampleMineField::~SampleMineField(void)
{
    // nothing to do
}

int SampleMineField::Alive(const Sample & query) const
{
    return (!_grid.Conflict(query, _conflict_checker));
}
