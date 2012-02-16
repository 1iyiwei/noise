/*
  EmptyMineField.cpp

  Li-Yi Wei
  11/09/2008

*/

#include "EmptyMineField.hpp"

EmptyMineField::EmptyMineField(void)
{
    // nothing to do
}

EmptyMineField::~EmptyMineField(void)
{
    // nothing to do
}

int EmptyMineField::Alive(const Sample & query) const
{
    // always survives
    return 1;
}
