/*
  Proximity2D.hpp

  the base class for all 2D nearest neighbor search

  Li-Yi Wei
  2/6/2014

*/

#ifndef _PROXIMITY_2D_HPP
#define _PROXIMITY_2D_HPP

#include "Delaunay.hpp"

class Proximity2D
{
public:

    typedef Delaunay::Real Real;
    typedef Delaunay::Point2D Point2D;

    virtual ~Proximity2D(void) = 0;

    virtual bool Nearest(const Point2D & query, Point2D & result) const;
};
#endif

