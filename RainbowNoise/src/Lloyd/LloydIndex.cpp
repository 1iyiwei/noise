/*
  LloydIndex.cpp

  Li-Yi Wei
  07/25/2009

*/

#include "LloydIndex.hpp"
#include "Exception.hpp"

LloydIndex::~LloydIndex(void)
{
    // nothing to do
}
    
LloydIndex * LloydIndex::Clone(void) const
{
    throw Exception("LloydIndex::Clone() shouldn't be called");
    return 0;
}

int LloydIndex::NumClass(void) const
{
    throw Exception("LloydIndex::NumClass() shouldn't be called");
    return 0;
}

void LloydIndex::Clear(void)
{
    throw Exception("LloydIndex::Clear() shouldn't be called");
}

void LloydIndex::Clear(const int num_class)
{
    throw Exception("LloydIndex::Clear() shouldn't be called");
}
    
int LloydIndex::Set(const int which_class, const int lord, const float dist)
{
    throw Exception("LloydIndex::Set() shouldn't be called");
    return 0;
}

int LloydIndex::Get(const int which_class, int & lord, float & dist) const
{
    throw Exception("LloydIndex::Get() shouldn't be called");
    return 0;
}
    
int LloydIndex::Set(const vector<int> & class_combo, const int lord, const float dist)
{
    throw Exception("LloydIndex::Set() shouldn't be called");
    return 0;
}

int LloydIndex::Get(const vector<int> & class_combo, int & lord, float & dist) const
{
    throw Exception("LloydIndex::Get() shouldn't be called");
    return 0;
}
