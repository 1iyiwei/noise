/*
  DelaunayMesh.hpp

  manager for core Delaunay class and its vertices and bounding box

  Li-Yi Wei
  2/7/2014

*/

#ifndef _DELAUNAY_MESH_HPP
#define _DELAUNAY_MESH_HPP

#include <string>
using namespace std;

#include "Delaunay.hpp"

class DelaunayMesh
{
public:
    typedef Delaunay::Real Real;
    typedef Delaunay::Point2D Point2D;
    typedef enum {NONE, TOROIDAL} BoundaryCondition;
    typedef Delaunay::AABB Box;

    DelaunayMesh(const Box & bounding_box, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D *> & points);

    DelaunayMesh(const Box & bounding_box, const Real very_small, const BoundaryCondition boundary_condition, const vector<const Point2D *> & points);

    virtual ~DelaunayMesh(void);

    Delaunay * Mesh(void);
    const Delaunay * Mesh(void) const;

    vector<Point2D *> & Vertices(void);
    const vector<Point2D *> & Vertices(void) const;

    vector<Point2D *> & Clones(void);
    const vector<Point2D *> & Clones(void) const;

    Box & InnerBox(void);
    const Box & InnerBox(void) const;

    Box & OuterBox(void);
    const Box & OuterBox(void) const;

protected:
    DelaunayMesh(const DelaunayMesh & original);
    DelaunayMesh & operator=(const DelaunayMesh & original);

    string CommonConstruct(const Box & bounding_box, const Real very_small, const BoundaryCondition boundary_condition, const vector<const Point2D *> & points);

    static string InsertSites(Delaunay & mesh, vector<Point2D *> & points);

protected:
    Delaunay *_mesh;
    vector<Point2D *> _vertices;
    vector<Point2D *> _clones;
    Box _inner_box;
    Box _outer_box;
};

#endif
