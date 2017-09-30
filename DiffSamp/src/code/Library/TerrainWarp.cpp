/*
  TerrainWarp.cpp

  Li-Yi Wei
  10/22/2010

*/

#include <math.h>

#include "TerrainWarp.hpp"
#include "Math.hpp"
#include "Exception.hpp"

TerrainWarp::TerrainWarp(void)
{
    // nothing else to do
}

TerrainWarp::~TerrainWarp(void)
{
    // nothing else to do
}

int TerrainWarp::ForwardMap(const Coordinate & source, Coordinate & target) const
{
    if(source.Dimension() < 2) return 0;

    target = source;

    target[0] = 0.25*(1 + 2*source[0] + sin(2*Math::PI*source[1]));
    
    return 1;
}

int TerrainWarp::InverseMap(const Coordinate & source, Coordinate & target) const
{
    if(source.Dimension() < 2) return 0;

    target = source;

    target[0] = 0.5*(4*source[0] - 1 - sin(2*Math::PI*source[1]));

    return 1;
}
    
int TerrainWarp::ForwardJacobian(const Coordinate & query, Matrix & jacobian) const
{
    const int dimension = query.Dimension();

    if(dimension < 2) return 0;

    if((jacobian.Size(0) != dimension) || (jacobian.Size(1) != dimension))
    {
        jacobian = Matrix(vector<int>(2, dimension));
    }

    jacobian.Identity();

    vector<int> index(2, 0);

    if(! jacobian.Put(index, 0.5)) return 0;

    index[1] = 1;
    if(! jacobian.Put(index, 0.5*Math::PI*cos(2*Math::PI*query[1]))) return 0;

    return 1;
}

int TerrainWarp::InverseJacobian(const Coordinate & query, Matrix & jacobian) const
{
    const int dimension = query.Dimension();

    if(dimension < 2) return 0;

    if((jacobian.Size(0) != dimension) || (jacobian.Size(1) != dimension))
    {
        jacobian = Matrix(vector<int>(2, dimension));
    }

    jacobian.Identity();

    vector<int> index(2, 0);

    if(! jacobian.Put(index, 2.0)) return 0;

    index[1] = 1;
    if(! jacobian.Put(index, -1.0*Math::PI*cos(2*Math::PI*query[1]))) return 0;

    return 1;
}
