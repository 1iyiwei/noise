/*
  PlainBoxDomain.hpp

  plain, non-toroidal, box domain
  
  Li-Yi Wei
  06/19/2009

*/

#ifndef _PLAIN_BOX_DOMAIN_HPP
#define _PLAIN_BOX_DOMAIN_HPP

#include "BoxDomain.hpp"

class PlainBoxDomain : public BoxDomain
{
public:
    PlainBoxDomain(const vector<float> & domain_size);
    ~PlainBoxDomain(void);
};

#endif
