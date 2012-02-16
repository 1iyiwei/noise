/*
  SingleBlob.hpp

  base class for per-sample single blob, e.g. Gaussian

  Li-Yi Wei
  06/15/2009

*/

#ifndef _SINGLE_BLOB_HPP
#define _SINGLE_BLOB_HPP

#include "Blob.hpp"

class SingleBlob : public Blob
{
public:
    virtual ~SingleBlob(void) = 0;

    // return 1 if success, 0 if failure
    int SetKernelSize(const float kernel_size);

    virtual float Get(const Sample & query) const;

    virtual SingleBlob * Clone(void) const;

protected:
    float _kernel_size;
};

#endif
