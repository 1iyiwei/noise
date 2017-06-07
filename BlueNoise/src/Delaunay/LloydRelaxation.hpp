/*
  LloydRelaxation.hpp

  Li-Yi Wei
  11/01/2007

*/

#ifndef _LLOYD_RELAXATION_HPP
#define _LLOYD_RELAXATION_HPP

#include <string>
using namespace std;

#include "Delaunay.hpp"

class LloydRelaxation
{
public:
    typedef Delaunay::Real Real;
    typedef Delaunay::Point2D Point2D;
    
    // give a bunch of points
    // perform relaxation once to determine the points' new positions
    // note that all point locations are determined in parallel
    // and are thus order-independent
    // the caller will own the output points
    // return "" if OK, or error message
    static string RunOnce(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const vector<Point2D *> & points);
    
protected:
    static int Centroid(const Point2D & center, const vector<const Point2D *> & neighbors, Point2D & centroid);
    
    static int VoronoiVertex(const Point2D & center, const Point2D & neighbor0, const Point2D & neighbor1, Point2D & result);
    
    static int Centroid(const vector<Point2D *> & polygon, Point2D & centroid);

    static Real Angle(const Point2D & point);

    struct Sortie
    {
        Sortie(void) : index(0), value(0) {};
        Sortie(const int index_i, const Real value_i) : index(index_i), value(value_i) {};

        int operator<(const Sortie & another) const {return value < another.value;};
        
        unsigned int index;
        Real value;
    };
};

#endif
