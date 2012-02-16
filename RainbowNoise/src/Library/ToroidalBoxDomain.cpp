/*
  ToroidalBoxDomain.cpp

  Li-Yi Wei
  06/19/2009

*/

#include <math.h>
#include "Math.hpp"
#include "ToroidalBoxDomain.hpp"

ToroidalBoxDomain::ToroidalBoxDomain(const vector<float> & domain_size) : BoxDomain(domain_size)
{
    // nothing else to do
}

ToroidalBoxDomain::~ToroidalBoxDomain(void)
{
    // nothing else to do
}

int ToroidalBoxDomain::Nearest(const Sample & anchor, const Sample & query, Sample & answer) const
{
    if(query.coordinate.Dimension() != anchor.coordinate.Dimension())
    {
        return 0;
    }

    if(query.coordinate.Dimension() != _domain_size.size())
    {
        return 0;
    }

    answer = query;

    for(int i = 0; i < answer.coordinate.Dimension(); i++)
    {
        const float diff = anchor.coordinate[i]-query.coordinate[i];
        answer.coordinate[i] = query.coordinate[i] + (fabs(diff) > 0.5*_domain_size[i])*(2*(diff > 0)-1)*_domain_size[i];
    }

    return 1;
}

int ToroidalBoxDomain::Adopt(const Sample & query, Sample & answer) const
{
    if(query.coordinate.Dimension() != _domain_size.size())
    {
        return 0;
    }

    answer = query;

    for(unsigned int i = 0; i < answer.coordinate.Dimension(); i++)
    {
        answer.coordinate[i] = mod(query.coordinate[i], _domain_size[i]);
    }

    return 1;
}
