/*
  DelaunayProximity2D.cpp

  Li-Yi Wei
  2/7/2014

*/

#include "DelaunayProximity2D.hpp"
#include "Math.hpp"

DelaunayProximity2D::DelaunayProximity2D(const Box & bounding_box, const Real very_small, const BoundaryCondition boundary_condition, const vector<const Point2D *> & points) : _mesh(bounding_box, very_small, boundary_condition, points), _boundary_condition(boundary_condition)
{
    // nothing else to do
}

DelaunayProximity2D::~DelaunayProximity2D(void)
{
    // nothing else to do
}

bool DelaunayProximity2D::Nearest(const Point2D & query, Point2D & result) const
{
    const Delaunay * mesh = _mesh.Mesh();

    if(mesh == 0) return false;

    if(_boundary_condition == DelaunayMesh::NONE)
    {
        return mesh->NearestSite(query, result);
    }
    else if(_boundary_condition == DelaunayMesh::TOROIDAL)
    {
        const DelaunayMesh::Box box = _mesh.InnerBox();

        const Real x_size = (box.x_max - box.x_min);
        const Real y_size = (box.y_max - box.y_min);
        const Real x_tile = floor(query.x/x_size);
        const Real y_tile = floor(query.y/y_size);

        const Point2D query_mod(Math::mod(query.x, x_size), Math::mod(query.y, y_size));

        if(! mesh->NearestSite(query_mod, result)) return false;

        result.x += x_tile*x_size;
        result.y += y_tile*y_size;

        return true;
    }
    else
    {
        return false;
    }
}
