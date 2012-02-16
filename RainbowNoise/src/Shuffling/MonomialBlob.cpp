/*
  MonomialBlob.cpp

  Li-Yi Wei
  09/05/2009

*/

#include <algorithm>
using namespace std;

#include <math.h>

#include "MonomialBlob.hpp"
#include "Exception.hpp"

MonomialBlob::MonomialBlob(const float degree): _degree(degree)
{
    if(degree <= 0)
    {
        throw Exception("MonomialBlob::MonomialBlob(): degree must be > 0");
    }
}

MonomialBlob::~MonomialBlob(void)
{
    // nothing else to do
}

float MonomialBlob::Get(const Sample & query) const
{
    float distance2 = 0;

    if(query.coordinate.Dimension() != _center.coordinate.Dimension())
    {
        throw Exception("MonomialBlob::Get(): dimension mismatch");
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

    distance2 = sqrt(distance2);

    return pow(max(0.0, 1.0 - distance2/_kernel_size), _degree);
}

MonomialBlob * MonomialBlob::Clone(void) const
{
    return (new MonomialBlob(*this));
}
