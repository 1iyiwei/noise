/*
  ToroidalBoxDomain.hpp

  toroidal box domain
  
  Li-Yi Wei
  06/19/2009

*/

#ifndef _TOROIDAL_BOX_DOMAIN_HPP
#define _TOROIDAL_BOX_DOMAIN_HPP

#include "BoxDomain.hpp"

class ToroidalBoxDomain : public BoxDomain
{
public:
    ToroidalBoxDomain(const vector<float> & domain_size);
    ~ToroidalBoxDomain(void);

    BoundaryCondition Boundary(void) const;

    int Nearest(const Sample & anchor, const Sample & query, Sample & answer) const; 

    int Adopt(const Sample & query, Sample & answer) const;
};

#endif
