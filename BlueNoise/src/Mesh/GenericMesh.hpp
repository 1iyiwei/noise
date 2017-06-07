/*
 * GenericMesh.hpp
 *
 * a generic template mesh class
 *
 * Li-Yi Wei
 * 9/19/00
 * 10/30/07 modified
 */

#ifndef _GENERIC_MESH_2D_HPP
#define _GENERIC_MESH_2D_HPP

#include <vector>
using namespace std;

#include "Mesh.hpp"
#include "MeshCommon.hpp"

template<class Vertex, class Face>
class GenericMesh: public Mesh
{
public:
    GenericMesh(void); 
    virtual ~GenericMesh(void);
 
    //GenericMesh & operator=(const GenericMesh & rhs);

    int GetVertices(vector<const Vertex *> & vertices) const;
    int GetFaces(vector<const Face *> & faces) const;
    int GetVertices(vector<Vertex *> & vertices);
    int GetFaces(vector<Face *> & faces);
    
    int InsertFace(const vector<Vertex *> & vertices, Face * face = 0);

protected:
    // remove unused vertices, and re-assign their IDs
    void CleanVertices(vector<Vertex *> & vertices) const;

private:
    // some temporary variables for avoiding dynamic memory allocation
    mutable vector<const Vertex *> _vertices_const;
    mutable vector<const Face *> _faces_const;
  
    //mutable vector<Vertex *> _vertices_nonconst;
    //mutable vector<Face *> _faces_nonconst;
    mutable vector<QuadEdge::Simplex *> _vertex_simplices;
    mutable vector<QuadEdge::Simplex *> _face_simplices;
};

template<class Vertex, class Face>
GenericMesh<Vertex, Face>::GenericMesh(void)
{
    // nothing to do
}

template<class Vertex, class Face>
GenericMesh<Vertex, Face>::~GenericMesh(void)
{ 
    // nothing to do
}

/*
template<class Vertex, class Face>
GenericMesh<Vertex, Face> & \
GenericMesh<Vertex, Face>::operator=(const GenericMesh<Vertex, Face> & rhs)
{
  cerr<<"GenericMesh::operator=(const GenericMesh & rhs) called"<<endl;
  throw Exception();
}
*/

template<class Vertex, class Face>
int GenericMesh<Vertex, Face>::InsertFace(const vector<Vertex *> & vertices, Face * face)
{ 
    vector<QuadEdge::Simplex *> simplices;

    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        simplices.push_back(vertices[i]);
    }

    if(! Mesh::InsertFace(simplices, face) )
    {
        return 0;
    }

    return 1;
}

template<class Vertex, class Face>
int GenericMesh<Vertex, Face>::GetVertices(vector<const Vertex *> & vertices) const
{
    //static vector<QuadEdge::Simplex *> _vertex_simplices;

    if(!Mesh::GetVertices(_vertex_simplices))
    {
        return 0;
    }
 
    if(vertices.size() != static_cast<int>(_vertex_simplices.size()))
    {
        vertices = vector<const Vertex *>(_vertex_simplices.size());
    }

    for(unsigned int i = 0; i < _vertex_simplices.size(); i++)
    {
        vertices[i] = my_dynamic_cast<const Vertex *>(_vertex_simplices[i]);
        
        if(vertices[i] == 0)
	{
            return 0;
	}
    }
 
    return 1;
}

template<class Vertex, class Face>
int GenericMesh<Vertex, Face>::GetFaces(vector<const Face *> & faces) const
{
    //static vector<QuadEdge::Simplex *> _face_simplices;
  
    if(!Mesh::GetFaces(_face_simplices))
    {
        return 0;
    }
 
    if(faces.size() != static_cast<int>(_face_simplices.size()))
    {
        faces = vector<const Face *>(_face_simplices.size());
    }

    for(unsigned int i = 0; i < _face_simplices.size(); i++)
    {
        faces[i] = my_dynamic_cast<const Face *>(_face_simplices[i]);

        if(faces[i] == 0)
	{
            return 0;
	}
    }
  
    return 1;
}

template<class Vertex, class Face>
int GenericMesh<Vertex, Face>::GetVertices(vector<Vertex *> & vertices)
{
    //static vector<const Vertex *> _vertices_const;

    if(! GetVertices(_vertices_const) )
    {
        return 0;
    }

    if(vertices.size() != _vertices_const.size())
    {
        vertices = vector<Vertex *>(_vertices_const.size());
    }

    for(int i = 0; i < vertices.size(); i++)
    {
        vertices[i] = const_cast<Vertex *>(_vertices_const[i]);
    }

    return 1;
}

template<class Vertex, class Face>
int GenericMesh<Vertex, Face>::GetFaces(vector<Face *> & faces)
{
    //static vector<const Face *> _faces_const;

    if(! GetFaces(_faces_const) )
    {
        return 0;
    }

    if(faces.size() != _faces_const.size())
    {
        faces = vector<Face *>(_faces_const.size());
    }

    for(int i = 0; i < faces.size(); i++)
    {
        faces[i] = const_cast<Face *>(_faces_const[i]);
    }

    return 1;
}

#endif
