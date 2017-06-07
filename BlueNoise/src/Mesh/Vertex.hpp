/*
 * Vertex.hpp
 *
 * the base vertex class
 *
 * Li-Yi Wei
 * 9/15/00
 * 10/30/07 modified
 */

#ifndef _VERTEX_HPP
#define _VERTEX_HPP

#include <vector>
using namespace std;

#include "QuadEdge.hpp"
#include "MeshCommon.hpp"

class Vertex : public QuadEdge::Simplex
{
public:
    // usually 2D or 3D points
    Vertex(void);
    virtual ~Vertex(void) = 0;

    // iterate through all the edges originating from this vertex
    void ResetEdgeIterator(const int whichRing = 0) const;
    const QuadEdge::Edge * NextEdge(void) const;

    // get the surrounding faces
    template<class FaceType>
    int GetFaces(vector<const FaceType *> & faces) const;

protected:
    // for the edge iterator
    mutable const QuadEdge::Edge * _nextEdge;
    mutable int _whichRing;
};

template<class FaceType>
int Vertex::GetFaces(vector<const FaceType *> & faces) const
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
    for(unsigned int i = 0; i < faces.size(); i++)
    {
        if(edge->GetLeft() != 0)
	{
            faces[i] = my_dynamic_cast<const FaceType*>(edge->GetLeft());
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
