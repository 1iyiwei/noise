/*
  LloydSample.cpp

  Li-Yi Wei
  07/15/2009

*/

#include "LLoydSample.hpp"
#include "Math.hpp"

LloydSample::LloydSample(void) : density(1)
{
    // nothing else to do
}

LloydSample::LloydSample(const int dimension, const LloydIndex & index_i) : sample(dimension), density(1), index(index_i)
{
    // nothing else to do
}

LloydSample::~LloydSample(void)
{
    // nothing else to do
}

LloydSample::Index::Index(const LloydIndex & index) : _index(index.Clone())
{
    // nothing else to do
}

LloydSample::Index::Index(void) : _index(0)
{
    // nothing else to do
}

LloydSample::Index::~Index(void)
{
    if(!_index)
    {
        delete _index;
        _index = 0;
    }
}

int LloydSample::Index::NumClass(void) const
{
    return (_index ? _index->NumClass() : 0);
}

void LloydSample::Index::Clear(void)
{
    if(_index) _index->Clear();
}

void LloydSample::Index::Clear(const int num_class)
{
    if(_index) _index->Clear(num_class);
}

int LloydSample::Index::Set(const int which_class, const int lord, const float dist)
{
    if(_index) return _index->Set(which_class, lord, dist);
    else return 0;
}

int LloydSample::Index::Get(const int which_class, int & lord, float & dist) const
{
    if(_index) return _index->Get(which_class, lord, dist);
    else return 0;
}

int LloydSample::Index::Set(const vector<int> & class_combo, const int lord, const float dist)
{
    if(_index) return _index->Set(class_combo, lord, dist);
    else return 0;
}

int LloydSample::Index::Get(const vector<int> & class_combo, int & lord, float & dist) const
{
    if(_index) return _index->Get(class_combo, lord, dist);
    else return 0;
}

int LloydSample::Index::Get(const vector<int> & class_combo) const
{
    int lord = -1;
    float dist = Math::INF;

    if(_index && _index->Get(class_combo, lord, dist))
    {
        return lord;
    }
    else
    {
        return -1;
    }
}

LloydSample::Index::Index(const Index & index) : _index(index._index ? index._index->Clone() : 0)
{
    // nothing else to do
}

LloydSample::Index & LloydSample::Index::operator=(const Index & rhs)
{
    if(_index)
    {
        delete _index;
        _index = 0;
    }

    _index = (rhs._index ? rhs._index->Clone() : 0);

    return *this;
}
