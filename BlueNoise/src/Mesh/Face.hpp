/*
 * Face.hpp
 *
 * the base face class
 *
 * Li-Yi Wei
 * 9/15/00
 * 10/30/07 modified
 */

#ifndef _FACE_HPP
#define _FACE_HPP

#include "QuadEdge.hpp"

class Face : public QuadEdge::Simplex
{
public:
    class Exception {};

public:
    // usually 2D or 3D points
    Face(void);
    virtual ~Face(void) = 0;

    // iterate through all the edges surrounding this face
    void ResetEdgeIterator(const int whichRing = 0) const;
    const QuadEdge::Edge * NextEdge(void) const;

    // get the list of vertices surrounding this face in CCW ordering
    //int GetVertices(vector<const Simplex *> & vertices) const;
    template<class Vertex>
    int GetVertices(vector<const Vertex *> & vertices) const;

    // get the list of faces surrounding this face in CCW ordering
    template<class FaceType>
    int GetFaces(vector<const FaceType *> & faces) const;

protected:
    // for the edge iterator
    mutable const QuadEdge::Edge * _nextEdge;
    mutable int _whichRing;
};

#include "MeshCommon.hpp"

template<class Vertex>
int Face::GetVertices(vector<const Vertex *> & vertices) const
{
#if 1
    // faster version
    const QuadEdge::Edge * edge = 0;

    int count = 0;

    if(_edges.size() > 0)
    {
        edge = _edges[0];
    }
    else
    {
        vertices.clear();
        return 1;
    }

    do
    {
        count++;
    }
    while( (edge = edge->Onext()) != _edges[0] );

    if(vertices.size() != count)
    {
        vertices = vector<const Vertex *>(count);
    }

    edge = _edges[0];
    for(int i = 0; i < vertices.size(); i++)
    {
        //vertices[i] = dynamic_cast<const Vertex*>(edge->GetOrigin());
        //vertices[i] = reinterpret_cast<const Vertex*>(edge->GetOrigin());
        vertices[i] = my_dynamic_cast<const Vertex*>(edge->GetRight());
        edge = edge->Onext();

        if(vertices[i] == 0)
        {
            return 0;
        }
    }

    return 1;

#else 
    // slower version
    ResetEdgeIterator();

    const QuadEdge::Edge * edge = 0;

    // count the number of vertices
    int count = 0;
    while( (edge = NextEdge()) != 0 )
    {
        count++;
    }

    if(vertices.size() != count)
    {
        vertices = vector<const Vertex *>(count);
    }

    ResetEdgeIterator();

    for(int i = 0; i < vertices.size(); i++)
    {
        edge = NextEdge();
        //vertices[i] = dynamic_cast<const Vertex*>(edge->GetOrigin());
        //vertices[i] = reinterpret_cast<const Vertex*>(edge->GetOrigin());
        vertices[i] = my_dynamic_cast<const Vertex*>(edge->GetOrigin());

        if(vertices[i] == 0)
	{
            return 0;
	}
    }

    return 1;
#endif
}

template<class FaceType>
int Face::GetFaces(vector<const FaceType *> & faces) const
{
    // faster version
    const QuadEdge::Edge * edge = 0;

    int count = 0;

    if(_edges.size() > 0)
    {
        edge = _edges[0];
    }
    else
    {
        faces.clear();
        return 1;
    }

    do
    {
        count++;
    }
    while( (edge = edge->Onext()) != _edges[0] );

    if(faces.size() != count)
    {
        faces = vector<const FaceType *>(count);
    }

    edge = _edges[0];
    for(int i = 0; i < faces.size(); i++)
    {
        if(edge->GetDestination() != 0)
	{
            faces[i] = my_dynamic_cast<const FaceType*>(edge->GetDestination());
            if(faces[i] == 0)
	    {
                return 0;
	    }
	}
        else
	{
            faces[i] = 0;
	}

        edge = edge->Onext();

    }

    return 1;
}
#endif
