/*
  KernelSize.hpp

  the base class for determining kernel size associated with each sample

  Li-Yi Wei
  06/09/2009

*/

#ifndef _KERNEL_SIZE_HPP
#define _KERNEL_SIZE_HPP

#include "Sample.hpp"

#define _USE_2_CLASS_ENERGY

class KernelSize
{
public:
    KernelSize(void);
    virtual ~KernelSize(void) = 0;

#ifdef _USE_2_CLASS_ENERGY
protected:
#endif
    // source_classes is a binary array indicating 
    // the presence/absence of individual classes
    // return kernel size, > 0 if ok, <= 0 if error
    // sample is for location only (adaptive sampling);
    // the sample.id info is not used
    float Get(const Sample & sample, const vector<int> & source_classes) const;
#ifdef _USE_2_CLASS_ENERGY
public:
#endif

    // similar to above, but has only two class id
    float Get(const Sample & sample, const int class_id0, const int class_id1) const;
    // similar to above, but the first id is implicit in center.id
    float Get(const Sample & center, const int query_class_id) const;

    virtual int NumClass(void) const;

    // default is 1.0
    int SetScale(const float value);

protected:
    // internal versions without knowing scale
    virtual float GetSize(const Sample & sample, const vector<int> & source_classes) const;
    virtual float GetSize(const Sample & sample, const int class_id0, const int class_id1) const;

protected:
    float _kernel_size_scale;
};

#endif
