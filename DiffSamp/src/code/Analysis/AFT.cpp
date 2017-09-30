/*
  AFT.cpp

  Li-Yi Wei
  08/30/2010

*/

#include "AFT.hpp"
#include "Exception.hpp"

AFT::AFT(const int num_samples, const float normalization_const) : _num_samples(num_samples), _normalization_const(normalization_const)
{
    // nothing else to do
}

AFT::~AFT(void)
{
    // nothing else to do
}

float AFT::Evaluate(const Coordinate & w) const
{
    // shouldn't be called
    throw Exception("PFT::Evaluate() should not be called!");
    return -1;
}
