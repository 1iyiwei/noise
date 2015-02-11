/*
  Coverage.cpp

  Li-Yi Wei
  10/15/2013
  
*/

#include <cmath>
using namespace std;

#include "Coverage.hpp"
#include "Math2D.hpp"
#include "Exception.hpp"

Coverage::~Coverage(void)
{
    // nothing else to do
}

Coverage::Real Coverage::Integral(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D> & points, const Real power) const
{
    throw Exception("Coverage::Integral(): shouldn't be called");
    return 0;
}

std::vector<Coverage::Vector2D> Coverage::Gradient(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D> & points, const Real power) const
{
    throw Exception("Coverage::Gradient(): shouldn't be called");
    return std::vector<Coverage::Vector2D>();
}

Coverage::Real Coverage::Mintegral(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const int num_points, const Real power) const
{
    throw Exception("Coverage::Mintegral(): shouldn't be called");
    return 0;
}
    
Coverage::Real Coverage::Distance(const Point2D & p1, const Point2D & p2)
{
    return sqrt(Math2D::Distance2(p1, p2));
}
