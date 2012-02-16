/*
  GaussianBlob.cpp

  Li-Yi Wei
  06/19/2009

*/

#include <math.h>

#include "GaussianBlob.hpp"
#include "Exception.hpp"

GaussianBlob::GaussianBlob(void)
{
    // nothing else to do
}

GaussianBlob::~GaussianBlob(void)
{
    // nothing else to do
}

float GaussianBlob::Get(const Sample & query) const
{
    float distance2 = 0;

    if(query.coordinate.Dimension() != _center.coordinate.Dimension())
    {
        throw Exception("GaussianBlob::Get(): dimension mismatch");
        return 0;
    }
    else
    {
        for(int i = 0; i < query.coordinate.Dimension(); i++)
        {
            const float diff = query.coordinate[i] - _center.coordinate[i];
            distance2 += diff*diff;
        }
    }

    return exp(-distance2/(_kernel_size*_kernel_size));
}

GaussianBlob * GaussianBlob::Clone(void) const
{
    return (new GaussianBlob(*this));
}
