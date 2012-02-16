/*
  ClassWeight.cpp

  Li-Yi Wei
  05/04/2009

*/

#include "ClassWeight.hpp"
#include "Exception.hpp"

ClassWeight::~ClassWeight(void)
{
    // nothing to do
}

int ClassWeight::Get(const vector<int> & source_classes, vector<float> & weights) const
{
    if(weights.size() != source_classes.size())
    {
        weights = vector<float>(source_classes.size());
    }

    for(unsigned int i = 0; i < weights.size(); i++)
    {
        weights[i] = Get(i, source_classes);

        if(weights[i] < 0) return 0;
    }

    return 1;
}

float ClassWeight::Get(const int class_id, const vector<int> & source_classes) const
{
    if((class_id < 0) || (class_id >= source_classes.size()))
    {
        return -1;
    }

    return Get(source_classes)*(source_classes[class_id] > 0);
}

float ClassWeight::Get(const int class_id0, const int class_id1) const
{
    throw Exception("ClassWeight::Get() should not be called");
    return -1;
}

float ClassWeight::Get(const vector<int> & source_classes) const
{
    throw Exception("ClassWeight::Get() should not be called");
    return -1;
}

int ClassWeight::NumClass(void) const
{
    throw Exception("ClassWeight::NumClass() should not be called");
    return 0;
}
