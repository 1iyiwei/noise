/*
  MultiLinearCoverage.hpp

  Li-Yi Wei
  11/07/2013

*/

#ifndef _MULTI_LINEAR_COVERAGE_HPP
#define _MULTI_LINEAR_COVERAGE_HPP

#include "VoronoiCoverage.hpp"

class MultiLinearCoverage : public VoronoiCoverage
{
protected:

    virtual Real TriIntegral(const Point2D & start, const Point2D & end, const Real power) const;

    virtual Vector2D TriIntegralGradient(const Point2D & start, const Point2D & end, const Real power) const;

    // multi-linear formulation
    static Real TriIntegralPlusEven(const Point2D & start, const Point2D & end, const int power);

    static Vector2D TriIntegralPlusEvenGradient(const Point2D & start, const Point2D & end, const int power);

    static Real CrossProduct(const Point2D & start, const Point2D & end)
    {
        return (start.x*end.y - end.x*start.y);
    }
};

#endif