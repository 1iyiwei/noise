/*
  KernelSize.cpp

  Li-Yi Wei
  06/09/2009

*/

#include "KernelSize.hpp"
#include "Exception.hpp"

KernelSize::KernelSize(void) : _kernel_size_scale(1.0)
{
    // nothing else to do
}

KernelSize::~KernelSize(void)
{
    // nothing else to do
}

float KernelSize::Get(const Sample & sample, const vector<int> & source_classes) const
{
    return GetSize(sample, source_classes)*_kernel_size_scale;
}

float KernelSize::Get(const Sample & sample, const int class_id0, const int class_id1) const
{
    return GetSize(sample, class_id0, class_id1)*_kernel_size_scale;
}

float KernelSize::Get(const Sample & sample, const int query_class_id) const
{
    return GetSize(sample, sample.id, query_class_id)*_kernel_size_scale;
}

int KernelSize::NumClass(void) const
{
    throw Exception("KernelSize::NumClass(): shouldn't be called!");
    return 0;
}

int KernelSize::SetScale(const float value)
{
    if(value >= 0)
    {
        _kernel_size_scale = value;
        return 1;
    }
    else
    {
        return 0;
    }
}

float KernelSize::GetSize(const Sample & sample, const vector<int> & source_classes) const
{
    throw Exception("KernelSize::GetSize(): shouldn't be called!");
    return -1;
}

float KernelSize::GetSize(const Sample & sample, const int class_id0, const int class_id1) const
{
    throw Exception("KernelSize::GetSize(): shouldn't be called!");
    return -1;
}
