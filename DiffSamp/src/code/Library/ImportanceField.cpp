/*
  ImportanceField.cpp

  Li-Yi Wei
  10/11/2010

*/

#include "ImportanceField.hpp"
#include "Exception.hpp"

ImportanceField::~ImportanceField(void)
{
    // nothing else to do
}

int ImportanceField::Dimension(void) const
{
    throw Exception("ImportanceField::Dimension(): shouldn't be called!");
    return 0;
}

float ImportanceField::Query(const Sample & sample) const
{
    throw Exception("ImportanceField::Query(): shouldn't be called!");
    return -1;
}
