/*
  DiffKernel.cpp

  Li-Yi Wei
  09/05/2010

*/

#include "DiffKernel.hpp"
#include "Exception.hpp"
 
DiffKernel::DiffKernel(void): _size(0)
{
    // nothing else to do
}

DiffKernel::~DiffKernel(void)
{
    // nothing else to do
}

void DiffKernel::SetSize(const float size)
{
    _size = size;
}

float DiffKernel::GetRadius(void) const
{
    throw Exception("DiffKernel::GetRadius(): shouldn't be called");
    return 0;
}

float DiffKernel::Get(const Coordinate & center, const Coordinate & query) const
{
    throw Exception("DiffKernel::Get(): shouldn't be called");
    return 0;
}
