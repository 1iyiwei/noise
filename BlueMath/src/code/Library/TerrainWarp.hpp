/*
  TerrainWarp.hpp

  u = x
  v = 0.25(1 + 2*y + sin(2*PI*x))

  Li-Yi Wei
  10/15/2010

*/

#ifndef _TERRAIN_WARP_HPP
#define _TERRAIN_WARP_HPP

#include "Warp.hpp"
#include "Matrix.hpp"

class TerrainWarp: public Warp
{
public:
    TerrainWarp(void);
    virtual ~TerrainWarp(void);

    int ForwardMap(const Coordinate & source, Coordinate & target) const;
    int InverseMap(const Coordinate & source, Coordinate & target) const;
    
    int ForwardJacobian(const Coordinate & query, Matrix & jacobian) const;
    int InverseJacobian(const Coordinate & query, Matrix & jacobian) const;
};      

#endif
