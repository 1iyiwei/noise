/*
  SampleCountClassWeight.cpp

  Li-Yi Wei
  07/19/2009

*/

#include "SampleCountClassWeight.hpp"
#include "Exception.hpp"

SampleCountClassWeight::SampleCountClassWeight(const vector<int> & num_samples_per_class) : _num_samples(num_samples_per_class)
{
    // nothing to do
}

SampleCountClassWeight::~SampleCountClassWeight(void)
{
    // nothing to do
}

float SampleCountClassWeight::Get(const vector<int> & source_classes) const
{
    throw Exception("SampleCountClassWeight::Get(): shouldn't be called");
    return -1;
}

int SampleCountClassWeight::NumClass(void) const
{
    return _num_samples.size();
}
