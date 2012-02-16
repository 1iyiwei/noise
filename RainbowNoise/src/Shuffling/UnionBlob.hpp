/*
  UnionBlob.hpp

  a collection of blobs centered at the same sample

  Li-Yi Wei
  06/15/2009

*/

#ifndef _UNION_BLOB_HPP
#define _UNION_BLOB_HPP

#include <memory>
using namespace std;

#include "SingleBlob.hpp"
#include "ClassWeight.hpp"
#include "KernelSize.hpp"

#include "SequentialCounter.hpp"

class UnionBlob : public Blob
{
public:
    UnionBlob(const SingleBlob & blob, const int num_classes, const ClassWeight & class_weight, const KernelSize & kernel_size);
    virtual ~UnionBlob(void);

    int SetCenter(const Sample & center);

    // class info query.id determines which class combinations to take
    // when query.id < 0, it is ignored
    // otherwise, consider only class set contains both blob.id and query.id
    float Get(const Sample & query) const;

    UnionBlob * Clone(void) const;

protected:
    const auto_ptr<SingleBlob> _p_blob;
    SingleBlob & _blob;

    const ClassWeight & _class_weight;
    const KernelSize & _kernel_size;

    mutable vector<int> _class_option;
    mutable SequentialCounter _counter;
};

#endif
