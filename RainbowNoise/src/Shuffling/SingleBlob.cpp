/*
  SingleBlob.cpp

  Li-Yi Wei
  06/15/2009

*/

#include "SingleBlob.hpp"
#include "Exception.hpp"

SingleBlob::~SingleBlob(void)
{
    // nothing else to do
}

int SingleBlob::SetKernelSize(const float kernel_size)
{
    _kernel_size = kernel_size;
    return 1;
}

float SingleBlob::Get(const Sample & query) const
{
    throw Exception("SingleBlob::Get() shouldn't be called");
    return 0;
}

SingleBlob * SingleBlob::Clone(void) const
{
    throw Exception("SingleBlob::Clone() shouldn't be called");
    return 0;
}
