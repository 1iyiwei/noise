/*
  Warp.cpp

  Li-Yi Wei
  10/15/2010

*/

#include "Warp.hpp"
#include "Exception.hpp"

Warp::~Warp(void)
{
    // nothing else to do
}

int Warp::ForwardMap(const Coordinate & source, Coordinate & target) const
{
    throw Exception("Warp::ForwardMap(): shouldn't be called");
    return 0;
}

int Warp::InverseMap(const Coordinate & source, Coordinate & target) const
{
    throw Exception("Warp::InverseMap(): shouldn't be called");
    return 0;
}

int Warp::ForwardJacobian(const Coordinate & query, Matrix & jacobian) const
{
    throw Exception("Warp::ForwardJacobian(): shouldn't be called");
    return 0;
}

int Warp::InverseJacobian(const Coordinate & query, Matrix & jacobian) const
{
    throw Exception("Warp::InverseJacobian(): shouldn't be called");
    return 0;
}
