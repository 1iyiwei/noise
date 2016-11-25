/*
  Filter.cpp

  Li-Yi Wei
  10/27/2010

*/

#include "Filter.hpp"
#include "Exception.hpp"

Filter::~Filter(void)
{
    // nothing else to do
}

float Filter::Query(const Array<float> & image, const vector<int> & index) const
{
    throw Exception("Filter::Query(): shouldn't be called!");

    return 0;
}
