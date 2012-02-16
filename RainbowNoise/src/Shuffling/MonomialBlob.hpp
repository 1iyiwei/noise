/*
  MonomialBlob.hpp

  Li-Yi Wei
  09/05/2009

*/

#ifndef _MONOMIAL_BLOB_HPP
#define _MONOMIAL_BLOB_HPP

#include "SingleBlob.hpp"

class MonomialBlob : public SingleBlob
{
public:
    MonomialBlob(const float degree);
    ~MonomialBlob(void);

    float Get(const Sample & query) const;

    MonomialBlob * Clone(void) const;

protected:
    const int _degree;
};

#endif
