/*
  DelaunayMesh.cpp

  Li-Yi Wei
  2/7/2014

*/

#include "DelaunayMesh.hpp"
#include "Exception.hpp"

#include <assert.h>

DelaunayMesh::DelaunayMesh(const Box & box, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D *> & points) : _mesh(0), _inner_box(box), _outer_box(box)
{
    vector<const Point2D *> input(points.size());
    for(unsigned int i = 0; i < input.size(); i++)
    {
        input[i] = points[i];
    }

    const string message = CommonConstruct(box, very_small, boundary_condition, input);

    if(message != "")
    {
        throw Exception("DelaunayMesh::DelaunayMesh()" + message);
    }    
}

DelaunayMesh::DelaunayMesh(const Box & box, const Real very_small, const BoundaryCondition boundary_condition, const vector<const Point2D *> & input) : _mesh(0), _inner_box(box), _outer_box(box)
{
    const string message = CommonConstruct(box, very_small, boundary_condition, input);

    if(message != "")
    {
        throw Exception("DelaunayMesh::DelaunayMesh()" + message);
    }    
}

string DelaunayMesh::CommonConstruct(const Box & box, const Real very_small, const BoundaryCondition boundary_condition, const vector<const Point2D *> & input)
{
    // compute delaunay triangulation of cloned output points
    _vertices = vector<Point2D *>(input.size());
    for(unsigned int i = 0; i < _vertices.size(); i++)
    {
        _vertices[i] = input[i]->Clone();
        assert(_vertices[i]);
    }

    _inner_box = box;

    const Real x_min = box.x_min;
    const Real x_max = box.x_max;
    const Real y_min = box.y_min;
    const Real y_max = box.y_max;
    
    // for toroidal boundary condition only
    if(boundary_condition == TOROIDAL)
    {
        for(unsigned int i = 0; i < input.size(); i++)
        {
            for(int row = -1; row <= 1; row++)
                for(int col = -1; col <= 1; col++)
                {
                    if((row != 0) || (col != 0)) // skip output itself
                    {
                        Point2D * baby = input[i]->Clone();

                        baby->x += row*(x_max-x_min);
                        baby->y += col*(y_max-y_min);

                        _clones.push_back(baby);
                    }
                }
        }
    }

    if(boundary_condition == TOROIDAL)
    {
        _outer_box = Box(2*x_min - x_max, 2*x_max - x_min, 2*y_min - y_max, 2*y_max - y_min);
    }
    else
    {
        _outer_box = Box(x_min, x_max, y_min, y_max);
    }

    _mesh = new Delaunay(_outer_box.x_min, _outer_box.x_max, _outer_box.y_min, _outer_box.y_max, very_small);

    {
        const string message = InsertSites(*_mesh, _vertices);
        if(message != "") 
        {
            return message;
        }

        if(_vertices.size() != input.size())
        {
            return "_vertices.size() != input.size(): some points are found to be duplicates!";
        }
    }
    {
        const string message = InsertSites(*_mesh, _clones);
        if(message != "") 
        {
            return message;
        }
    }

    return "";
}

DelaunayMesh::~DelaunayMesh(void)
{
    delete _mesh;

    for(unsigned int i = 0; i < _vertices.size(); i++)
    {
        delete _vertices[i];
    }
    _vertices.clear();

    for(unsigned int i = 0; i < _clones.size(); i++)
    {
        delete _clones[i];
    }
    _clones.clear();
}

Delaunay * DelaunayMesh::Mesh(void)
{
    return _mesh;
}

const Delaunay * DelaunayMesh::Mesh(void) const
{
    return _mesh;
}

vector<DelaunayMesh::Point2D *> & DelaunayMesh::Vertices(void)
{
    return _vertices;
}

const vector<DelaunayMesh::Point2D *> & DelaunayMesh::Vertices(void) const
{
    return _vertices;
}

vector<DelaunayMesh::Point2D *> & DelaunayMesh::Clones(void)
{
    return _clones;
}

const vector<DelaunayMesh::Point2D *> & DelaunayMesh::Clones(void) const
{
    return _clones;
}

DelaunayMesh::Box & DelaunayMesh::InnerBox(void)
{
    return _inner_box;
}

const DelaunayMesh::Box & DelaunayMesh::InnerBox(void) const
{
    return _inner_box;
}

DelaunayMesh::Box & DelaunayMesh::OuterBox(void)
{
    return _outer_box;
}

const DelaunayMesh::Box & DelaunayMesh::OuterBox(void) const
{
    return _outer_box;
}

DelaunayMesh::DelaunayMesh(const DelaunayMesh & original)
{
    throw Exception("DelaunayMesh::DelaunayMesh(): shouldn't be called");
}

DelaunayMesh & DelaunayMesh::operator=(const DelaunayMesh & original)
{
    throw Exception("elaunayMesh::operator=(): shouldnt be called");

    return *this;
}

string DelaunayMesh::InsertSites(Delaunay & mesh, vector<Point2D *> & input)
{
    string answer = "";

    for(unsigned int i = 0; i < input.size(); i++)
    {
        // cerr << "insert " << i << "/" << input.size() << " : (" <<  input[i]->id << ", " << input[i]->x << ", " << input[i]->y << ")" << endl; // debug

        switch(mesh.InsertSite(*input[i]))
        {
        case Delaunay::INSERT_OK:
            // nothing else to do
            break;

        case Delaunay::INSERT_DUPLICATE:
            input[i]->category = Point2D::ORPHAN;
            break;

        case Delaunay::INSERT_FAIL:
            // answer = "cannot insert site";
            input[i]->category = Point2D::ORPHAN;
            break;

        default:
            throw Exception("LloydRelaxation::InsertSites(): unhandled insert site case");
        }
    }

#if 0
    vector<Point2D *> output;
    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(input[i])
        {
            output.push_back(input[i]);
        }
    }
    input = output;
#endif

    return answer;
}
