/*
  BoxDomain.hpp

  the base class for all domains shaped in (axis-aligned) box

  Li-Yi Wei
  06/19/2009

*/

#ifndef _BOX_DOMAIN_HPP
#define _BOX_DOMAIN_HPP

#include "Domain.hpp"

class BoxDomain : public Domain
{
public:
    BoxDomain(const vector<float> & domain_size);
    virtual ~BoxDomain(void) = 0;

    int Dimension(void) const;

    int Inside(const Sample & query) const;

    int BoundingBox(vector<float> & answer) const;
    int UniformRandom(Sample & answer) const;

protected:
    const vector<float> _domain_size;
};

#endif
