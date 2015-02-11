/*
  Coverage.hpp

  See the image plane coverage section of the paper

  Li-Yi Wei
  10/15/2013

*/

#ifndef _COVERAGE_HPP
#define _COVERAGE_HPP

#include <vector>
using namespace std;

#include "LloydRelaxation.hpp"

class Coverage
{
public:
    typedef LloydRelaxation::Real Real;
    typedef LloydRelaxation::Point2D Point2D;
    typedef LloydRelaxation::BoundaryCondition BoundaryCondition;
    typedef LloydRelaxation::Vector2D Vector2D;

    virtual ~Coverage(void) = 0;

    virtual Real Integral(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D> & points, const Real power) const;

    // 2N-D gradient of the integral
    virtual std::vector<Vector2D> Gradient(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D> & points, const Real power) const;

    // optimal (min) value based on the theoretical uniform hex grids
    virtual Real Mintegral(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const int num_points, const Real power) const;

protected:
    static Real Distance(const Point2D & p1, const Point2D & p2);
};
#endif
