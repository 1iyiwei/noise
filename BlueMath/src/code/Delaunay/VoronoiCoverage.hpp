/*
  VoronoiCoverage.hpp

  the base class for all coverage computing classes that use Voronoi diagram

  Li-Yi Wei
  11/07/2013

*/

#ifndef _VORONOI_COVERAGE_HPP
#define _VORONOI_COVERAGE_HPP

#include "Coverage.hpp"

class VoronoiCoverage : public Coverage
{
public:
    typedef LloydRelaxation::VoronoiRegion Polygon;

    virtual ~VoronoiCoverage(void) = 0;

    virtual Real Integral(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D> & points, const Real power) const;

    virtual vector<Vector2D> Gradient(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D> & points, const Real power) const;

    // optimal value based on the theoretical uniform hex grids
    virtual Real Mintegral(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const int num_points, const Real power) const;

    // utility functions for optimization
    Real EnergyFunction(const Polygon & polygon, const Real power) const
    {
        return PolyIntegral(polygon, power);
    }

    Vector2D EnergyFunctionGradient(const Polygon & polygon, const Real power) const
    {
        return PolyIntegralGradient(polygon, power);
    }


protected:

    struct Mesh
    {
        vector<Polygon> polygons;
    };

    // compute 1/power normalized integral of r^{power} over all polygons
    virtual Real MeshIntegral(const Mesh & mesh, const Real power) const;

    virtual std::vector<Vector2D> MeshGradient(const Mesh & mesh, const Real power) const;

    // find the maximal distance from center to a vertex
    static Real Max(const Polygon & polygon);

    // compute integral of r^{power} over polygon 
    virtual Real PolyIntegral(const Polygon & polygon, const Real power) const;

    // compute integral of r^{power} over triangle (0, start, end)
    virtual Real TriIntegral(const Point2D & start, const Point2D & end, const Real power) const;

    // compute the gradient of PolyIntegral
    virtual Vector2D PolyIntegralGradient(const Polygon & polygon, const Real power) const;

    // compute integral of TriIntegral
    virtual Vector2D TriIntegralGradient(const Point2D & start, const Point2D & end, const Real power) const;
};

#endif