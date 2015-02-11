/*
  DelaunayProximity2D.hpp

  finding nearest neighbor via Delaunay triangulation

  Li-Yi Wei
  2/6/2014

*/

#ifndef _DELAUNAY_PROXIMITY_2D_HPP
#define _DELAUNAY_PROXIMITY_2D_HPP

#include <vector>
using namespace std;

#include "Proximity2D.hpp"
#include "DelaunayMesh.hpp"

class DelaunayProximity2D : public Proximity2D
{
public:    
    typedef DelaunayMesh::Real Real;
    typedef DelaunayMesh::Point2D Point2D;
    typedef DelaunayMesh::BoundaryCondition BoundaryCondition;
    typedef DelaunayMesh::Box Box;
    
    DelaunayProximity2D(const Box & bounding_box, const Real very_small, const BoundaryCondition boundary_condition, const vector<const Point2D *> & points);

    virtual ~DelaunayProximity2D(void);

    bool Nearest(const Point2D & query, Point2D & result) const;

protected:
    const DelaunayMesh _mesh;
    const BoundaryCondition _boundary_condition;
};

#endif
