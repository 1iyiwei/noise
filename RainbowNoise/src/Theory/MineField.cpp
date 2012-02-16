/*
  MineField.cpp

  Li-Yi Wei
  11/09/2008

*/

#include "MineField.hpp"
#include "Exception.hpp"

MineField::MineField(void)
{
    // nothing to do
}

MineField::~MineField(void)
{
    // nothing to do
}

int MineField::Alive(const Sample & query) const
{
    throw Exception("MineField::Alive() should not be called");

    return 0;
}
