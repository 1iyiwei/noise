/*
  Haar wavelet warp based on wavelet importance sampling in SIGGRAPH 2005

  Li-Yi Wei
  12/30/2010
  
*/

#ifndef _HAAR_WARP_HPP
#define _HAAR_WARP_HPP

#include <deque>
using namespace std;

#include "Warp.hpp"
#include "Array.hpp"

class HaarWarp: public Warp
{
public:
    HaarWarp(const Array<float> & importance, const float cell_size);
    virtual ~HaarWarp(void);

    int ForwardMap(const Coordinate & source, Coordinate & target) const;
    int InverseMap(const Coordinate & source, Coordinate & target) const;
    
    int ForwardJacobian(const Coordinate & query, Matrix & jacobian) const;
    int InverseJacobian(const Coordinate & query, Matrix & jacobian) const;

protected:
    struct Level
    {
        Level(void) : cell_size(0) {};

        Array<float> data;
        float cell_size;
    };

    static int BuildPyramid(const Array<float> & importance, const float cell_size, deque<Level> & pyramid);

    // forward map within one level
    int ForwardMap(const Level & level, const Coordinate & source, Coordinate & target) const;

    // fractional value divider
    float Divider(const Level & level, const vector<int> & index, const int which_dim) const;

protected:
    // level 0 is the lowest resolution with just one pixel
    deque<Level> _pyramid;
};      

#endif
