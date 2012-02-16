/*
  SoloLloydIndex.cpp

  Li-Yi Wei
  07/25/2009

*/

#include "SoloLloydIndex.hpp"
#include "Math.hpp"
#include "Exception.hpp"

SoloLloydIndex::SoloLloydIndex(const int num_class) : _data(num_class)
{
    // nothing else to do
}

SoloLloydIndex::SoloLloydIndex(void)
{
    // nothing else to do
}

SoloLloydIndex::~SoloLloydIndex(void)
{
    // nothing else to do
}

SoloLloydIndex * SoloLloydIndex::Clone(void) const
{
    return new SoloLloydIndex(*this);
}

int SoloLloydIndex::NumClass(void) const
{
    return _data.size();
}

void SoloLloydIndex::Clear(void)
{
    for(unsigned int i = 0; i < _data.size(); i++)
    {
        _data[i].lord = -1;
        _data[i].dist = Math::INF;
    } 
}

void SoloLloydIndex::Clear(const int num_class)
{
    _data = vector<Data>(num_class);
}

int SoloLloydIndex::Set(const int which_class, const int lord, const float dist)
{
    if((which_class < 0) || (which_class >= _data.size()))
    {
        return 0;
    }
    else
    {
        _data[which_class].lord = lord;
        _data[which_class].dist = dist;
        return 1;
    }
}

int SoloLloydIndex::Get(const int which_class, int & lord, float & dist) const
{
    if((which_class < 0) || (which_class >= _data.size()))
    {
        return 0;
    }
    else
    {
        lord = _data[which_class].lord;
        dist = _data[which_class].dist;
        return 1;
    }
}

int SoloLloydIndex::Set(const vector<int> & class_combo, const int lord, const float dist)
{
    throw Exception("SoloLloydIndex::Set(combo): shouldn't be called");
    return 0;
}

int SoloLloydIndex::Get(const vector<int> & class_combo, int & lord, float & dist) const
{
    lord = -1;
    dist = Math::INF;

    for(unsigned int i = 0; i < class_combo.size(); i++)
    {
        if(class_combo[i] && (_data[i].dist < dist))
        {
            lord = _data[i].lord;
            dist = _data[i].dist;
        }
    }

    return (lord >= 0);
}

#if 0
int SoloLloydIndex::Get(const vector<int> & class_combo) const
{
    int lord = -1;
    float dist = Math::INF;

    if(!Get(class_combo, lord, dist)) lord = -1;

    return lord;
}
#endif
