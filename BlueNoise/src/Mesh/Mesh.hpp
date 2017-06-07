/*
 * Mesh.hpp
 *
 * the mesh which uses quad edge data structures
 * the abstract base class
 * 
 * Li-Yi Wei
 * 9/8/00
 * 10/30/07 modified
 */

#ifndef _MESH_HPP
#define _MESH_HPP

#include <vector>
using namespace std;

#include "QuadEdge.hpp"

class Mesh
{
public:
    class Exception{};

public:
    Mesh(void);
    virtual ~Mesh(void) = 0;

    const QuadEdge::Edge * GetFirstEdge(void) const;

    int GetEdges(vector<const QuadEdge::Edge *> & edges) const;

protected:

    // get the list of vertices or faces of this mesh
    // return 1 if successful, 0 else
    int GetVertices(vector<QuadEdge::Simplex *> & vertices) const;
    int GetFaces(vector<QuadEdge::Simplex *> & faces) const;

    // for all the Insert* functions below
    // the item being inserted will directly become this class's property
    // in other words they are NOT cloned

    // insert the quad edge structure into the _edges 
    int InsertEdge(QuadEdge::Edge & edge);

    // insert a face by specifying the list of vertices in CCW order
    // it will also update the _edges structure
    // return 1 if successful, 0 else
    //
    // Because of the knowledge of a face, we can easily splice the edges
    int InsertFace(const vector<QuadEdge::Simplex *> & vertices,
                   QuadEdge::Simplex * face = 0);

    // find the edge from s1 to s2
    // return 0 if no such edge exist
    const QuadEdge::Edge * FindEdge(const QuadEdge::Simplex & s1,
                                    const QuadEdge::Simplex & s2) const;

private:
    Mesh(const Mesh & rhs);
    Mesh & operator=(const Mesh & rhs);

protected:
    // all pointers are properties of this class
    vector<QuadEdge::Edge *> _edges;
};

#endif
