/*
  GaussianBlob.hpp

  Li-Yi Wei
  06/19/2009

*/

#ifndef _GAUSSIAN_BLOB_HPP
#define _GAUSSIAN_BLOB_HPP

#include "SingleBlob.hpp"

class GaussianBlob : public SingleBlob
{
public:
    GaussianBlob(void);
    ~GaussianBlob(void);

    float Get(const Sample & query) const;

    GaussianBlob * Clone(void) const;
};

#endif
