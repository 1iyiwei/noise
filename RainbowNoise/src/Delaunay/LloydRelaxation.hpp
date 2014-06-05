/*
  LloydRelaxation.hpp

  Li-Yi Wei
  11/01/2007

*/

#ifndef _LLOYD_RELAXATION_HPP
#define _LLOYD_RELAXATION_HPP

#include <string>
using namespace std;

#include "DelaunayMesh.hpp"

class LloydRelaxation
{
public:
    typedef Delaunay::Real Real;
    typedef Delaunay::Point2D Point2D;
    typedef DelaunayMesh::BoundaryCondition BoundaryCondition;
    typedef Delaunay::Vector2D Vector2D;

    // give a bunch of points
    // perform relaxation once to determine the points' new positions
    // note that all point locations are determined in parallel
    // and are thus order-independent
    // the caller will own the output points
    // return "" if OK, or error message
    static string RunOnce(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const vector<Point2D *> & points);
    static string RunOnce(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D *> & points);

    struct VoronoiRegion
    {
        Point2D center;
        vector<Point2D> ring;
    };

    // compute the Voronoi regions
    static string Voronoi(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const vector<const Point2D *> & points, vector<VoronoiRegion> & output);

    static string Voronoi(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<const Point2D *> & points, vector<VoronoiRegion> & output);

    static string Voronoi(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const bool with_clone, const vector<const Point2D *> & points, vector<VoronoiRegion> & output);

    static string Centroid(const VoronoiRegion & region, Point2D & centroid);

    static Real Area(const VoronoiRegion & region);

    // snap the set of points to snappers
    // i.e. the allowable set of sample poisitions is discrete
    static string Snap(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const vector<Point2D *> & points, const vector<const Point2D *> & snappers);

    // compute the largest empty circle in from the Voronoi diagram
    static string LargestEmptyCircle(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const vector<VoronoiRegion> &regions, Real &x, Real &y, Real &radius);

protected:
    static Real Distance2(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const Point2D & p1, const Point2D & p2);

    static int Centroid(const Point2D & center, const vector<const Point2D *> & neighbors, Point2D & centroid);
    
    static int VoronoiVertex(const Point2D & center, const Point2D & neighbor0, const Point2D & neighbor1, Point2D & result);
    
    static int Centroid(const vector<Point2D *> & polygon, Point2D & centroid);

    // clip points outside the domain
    // return number of points clipped, negative for error
    static int Clip(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, vector<Point2D *> & points);

protected:
    struct Sortie
    {
        Sortie(void) : index(0), value(0) {};
        Sortie(const int index_i, const Real value_i) : index(index_i), value(value_i) {};

        int operator<(const Sortie & another) const {return value < another.value;};
        
        unsigned int index;
        Real value;
    };

    static Real Perturb(const Real value, const Real minimum, const Real maximum, const Real very_small);

    static const Real _VERY_SMALL;
};

#endif
