/*
  ComboLloydIndex.cpp

  Li-Yi Wei
  07/25/2009

*/

#include "ComboLloydIndex.hpp"
#include "Math.hpp"
#include "Exception.hpp"

ComboLloydIndex::ComboLloydIndex(const int num_class) : _data(vector<int>(num_class, 2))
{
    // nothing else to do
}

ComboLloydIndex::ComboLloydIndex(void)
{
    // nothing else to do
}

ComboLloydIndex::~ComboLloydIndex(void)
{
    // nothing else to do
}

ComboLloydIndex * ComboLloydIndex::Clone(void) const
{
    return new ComboLloydIndex(*this);
}

int ComboLloydIndex::NumClass(void) const
{
    return _data.Dimension();
}

void ComboLloydIndex::Clear(void)
{
    Data data;
    _data.Clear(data);
}

void ComboLloydIndex::Clear(const int num_class)
{
    _data = Array<Data>(vector<int>(num_class, 2));
}

int ComboLloydIndex::Set(const int which_class, const int lord, const float dist)
{
    throw Exception("ComboLloydIndex::Set(solo) shouldn't be called");
    return 0;
}

int ComboLloydIndex::Get(const int which_class, int & lord, float & dist) const
{
    throw Exception("ComboLloydIndex::Get(solo) shouldn't be called");
    return 0;
}

int ComboLloydIndex::Set(const vector<int> & class_combo, const int lord, const float dist)
{
    _item.lord = lord;
    _item.dist = dist;

    return _data.Put(class_combo, _item);
}

int ComboLloydIndex::Get(const vector<int> & class_combo, int & lord, float & dist) const
{
    if(! _data.Get(class_combo, _item))
    {
        return 0;
    }
    else
    {
        lord = _item.lord;
        dist = _item.dist;
        return 1;
    }
}
