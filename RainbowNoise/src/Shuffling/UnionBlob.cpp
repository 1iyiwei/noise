/*
  UnionBlob.cpp

  Li-Yi Wei
  06/15/2009

*/

#include "UnionBlob.hpp"
#include "Exception.hpp"

UnionBlob::UnionBlob(const SingleBlob & blob, const int num_classes, const ClassWeight & class_weight, const KernelSize & kernel_size) : _p_blob(blob.Clone()), _blob(*_p_blob), _class_weight(class_weight), _kernel_size(kernel_size), _class_option(vector<int>(num_classes)), _counter(num_classes, 0, 1)
{
    if(! _p_blob.get())
    {
        throw Exception("UnionBlob::UnionBlob(): empty blob pointer");
    }
}

UnionBlob::~UnionBlob(void)
{
    // nothing else to do
}

int UnionBlob::SetCenter(const Sample & center)
{
    if(! Blob::SetCenter(center)) return 0;
    
    if(! _blob.SetCenter(center)) return 0;
    
    if((center.id < 0) || (center.id >= _class_option.size()))
    {
        return 0;
    }

    return 1;
}

float UnionBlob::Get(const Sample & query) const
{
    float value = 0;

#ifdef _USE_2_CLASS_ENERGY
    const float weight = _class_weight.Get(_center.id, query.id);
    const float ksize = _kernel_size.Get(_center, query.id);
    
    if(weight < 0)
    {
        throw Exception("UnionBlob::Get(): weight < 0");
        return 0;
    }

    if(!_blob.SetKernelSize(ksize))
    {
        throw Exception("UnionBlob::Get(): cannot set kernel size");
        return 0;
    }

    value += weight*_blob.Get(query);
#else
    _counter.Reset();

    do
    {
        _counter.Get(_class_option);

        if(_class_option[_center.id] && ((query.id < 0) || (query.id >= _class_option.size()) || _class_option[query.id]))
        {
            const float weight = _class_weight.Get(_center.id, _class_option);
            const float ksize = _kernel_size.Get(_center, _class_option);
        
            if(weight < 0)
            {
                throw Exception("UnionBlob::Get(): weight < 0");
                return 0;
            }

            if(!_blob.SetKernelSize(ksize))
            {
                throw Exception("UnionBlob::Get(): cannot set kernel size");
                return 0;
            }

            value += weight*_blob.Get(query);
        }
    }
    while(_counter.Next());
#endif

    return value;
}

UnionBlob * UnionBlob::Clone(void) const
{
    throw Exception("UnionBlob::Clone() shouldn't be called");

    UnionBlob * baby = new UnionBlob(_blob, _class_option.size(), _class_weight, _kernel_size);
    return baby;
}
