/*
  Blob.cpp

  Li-Yi Wei
  06/15/2009

*/

#include "Blob.hpp"
#include "Exception.hpp"

Blob::~Blob(void)
{
    // nothing else to do
}

int Blob::SetCenter(const Sample & center)
{
    _center = center;
    return 1;
}

float Blob::Get(const Sample & query) const
{
    throw Exception("Blob::Get() shouldn't be called");
    return 0;
}

Blob * Blob::Clone(void) const
{
    throw Exception("Blob::Clone() shouldn't be called");
    return 0;
}
