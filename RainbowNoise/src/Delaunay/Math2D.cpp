/*
  Math2D.cpp

  Li-Yi Wei
  10/23/2013

*/

#include <cmath>
using namespace std;

#include "Math2D.hpp"
#include "Exception.hpp"

Math2D::Real Math2D::Distance2(const Point2D & p1, const Point2D & p2)
{
    const Real x_diff = p1.x - p2.x;
    const Real y_diff = p1.y - p2.y;
    return x_diff*x_diff + y_diff*y_diff;
}

Math2D::Real Math2D::Angle(const Point2D & point)
{
    Delaunay::Vector2D normalized(point.x, point.y);
    normalized.normalize();
    
    return (normalized.y >= 0 ? acos(normalized.x) : 2*acos(-1.0)-acos(normalized.x));
}

Math2D::Real Math2D::Area(const Point2D & p0, const Point2D & p1, const Point2D & p2)
{
    return 0.5*(p0.x*p1.y - p1.x*p0.y + p1.x*p2.y - p2.x*p1.y + p2.x*p0.y - p0.x*p2.y);
}
