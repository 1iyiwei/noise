/*
  Math2D.hpp

  Li-Yi Wei
  10/23/2013

*/

#ifndef _MATH_2D_HPP
#define _MATH_2D_HPP

#include "Delaunay.hpp"

class Math2D
{
public:
    typedef Delaunay::Real Real;
    typedef Delaunay::Point2D Point2D;
    
public:
    static Real Distance2(const Point2D & p1, const Point2D & p2);

    static Real Angle(const Point2D & point);

    // (signed) triangle area
    static Real Area(const Point2D & p0, const Point2D & p1, const Point2D & p2);

};

#endif
