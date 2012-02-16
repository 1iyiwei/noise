/*
  BoxDomain.cpp

  Li-Yi Wei
  06/19/2009

*/

#include "BoxDomain.hpp"
#include "Random.hpp"
#include "Exception.hpp"

BoxDomain::BoxDomain(const vector<float> & domain_size): _domain_size(domain_size)
{
    // nothing else to do
}

BoxDomain::~BoxDomain(void)
{
    // nothing else to do
}

int BoxDomain::Dimension(void) const
{
    return _domain_size.size();
}

int BoxDomain::Inside(const Sample & query) const
{
    if(query.coordinate.Dimension() != _domain_size.size())
    {
        throw Exception("BoxDomain::Inside(): dimension mismatch");
        return 0;
    }
    else
    {
        for(int i = 0; i < query.coordinate.Dimension(); i++)
        {
            if((query.coordinate[i] < 0) || (query.coordinate[i] >= _domain_size[i]))
            {
                return 0;
            }
        }

        return 1;
    }
}

int BoxDomain::BoundingBox(vector<float> & answer) const
{
    answer = _domain_size;
    return 1;
}

int BoxDomain::UniformRandom(Sample & answer) const
{
    if(answer.coordinate.Dimension() != _domain_size.size())
    {
        answer.coordinate = Coordinate(_domain_size.size());
    }

    for(int i = 0; i < answer.coordinate.Dimension(); i++)
    {
        answer.coordinate[i] = Random::UniformRandom()*_domain_size[i];
    }

    return 1;
}
