/*
  PFT.cpp

  Li-Yi Wei
  08/21/2008

*/

#include "PFT.hpp"
#include "Exception.hpp"

PFT::PFT(void)
{
    // nothing to do
}

PFT::~PFT(void)
{
    // nothing to do
}
   
float PFT::Evaluate(const Coordinate & w) const
{
    // shouldn't be called
    throw Exception("PFT::Evaluate() should not be called!");
    return -1;
}
    
PFT::Value::Value(void) : r(0), i(0)
{
    // nothing to do
}

PFT::Value::Value(const float r_in, const float i_in) : r(r_in), i(i_in)
{
    // nothing to do
}
