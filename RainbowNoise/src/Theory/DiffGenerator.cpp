/*
  DiffGenerator.cpp

  Li-Yi Wei
  10/30/2008

*/

#include "DiffGenerator.hpp"
#include "Exception.hpp"

DiffGenerator::DiffGenerator(const vector<float> & domain_size) : _domain_size(domain_size)
{
    // nothing to do
}

DiffGenerator::~DiffGenerator(void)
{
    // nothing to do
}

int DiffGenerator::Produce(vector<Sample> & differences) const
{
    // shouldn't be called
    throw Exception("DiffGenerator::Produce() should not be called");

    return 0;
}
