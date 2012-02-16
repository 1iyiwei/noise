/*
  Blob.hpp

  base class for all kinds of per sample blob
  including SingleBlob and UnionBlob

  Li-Yi Wei
  06/15/2009

*/

#ifndef _BLOB_HPP
#define _BLOB_HPP

#include "Sample.hpp"

class Blob
{
public:
    virtual ~Blob(void) = 0;

    // return 1 if success, 0 if failure
    virtual int SetCenter(const Sample & center);

    // evaluation
    virtual float Get(const Sample & query) const;

    // clone oneself
    virtual Blob * Clone(void) const;

protected:
    Sample _center;
};

#endif
