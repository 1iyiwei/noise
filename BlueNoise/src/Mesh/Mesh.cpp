/*
 * Mesh.cpp
 *
 * Li-Yi Wei
 * 9/8/00
 * 10/30/07 modified
 */

#include <iostream>
using namespace std;

#include "Mesh.hpp"

Mesh::Mesh(void)
{
    // nothing to do
}

Mesh::~Mesh(void)
{ 
#if 1
    // delete edges and the corresponding vertices/faces
    vector<QuadEdge::Simplex *> vertices;
    vector<QuadEdge::Simplex *> faces;
  
    if(! GetVertices(vertices) )
    {
        throw Exception();
    }
  
    if(! GetFaces(faces) )
    {
        throw Exception();
    }
 
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        delete vertices[i];
    }

    for(unsigned int i = 0; i < faces.size(); i++)
    {
        delete faces[i];
    }

    for(unsigned int i = 0; i < _edges.size(); i++)
    {
        delete _edges[i]->QEdge();
    }
#else
    // a more sophisticated (but much slower) version for deletion
  for(unsigned int i = 0; i < _edges.size(); i++)
  { 
      QuadEdge::DeleteFaceEdge(_edges[i]);
      //QuadEdge::DeleteVertexEdge(_edges[i]);
  }
#endif
}

const QuadEdge::Edge * Mesh::GetFirstEdge(void) const
{
    if(_edges.size() > 0)
    {
        return _edges[0];
    }
    else
    {
        return 0;
    }
}

int Mesh::GetEdges(vector<const QuadEdge::Edge *> & edges) const
{
    edges = vector<const QuadEdge::Edge *>(_edges.size());

    for(unsigned int i = 0; i < edges.size(); i++)
    {
        edges[i] = _edges[i];
    }

    return 1;
}

int Mesh::GetVertices(vector<QuadEdge::Simplex *> & simplices) const
{
    simplices.clear();
 
    for(unsigned int i = 0; i < _edges.size(); i++)
    { 
        const QuadEdge::Edge * edge = _edges[i];
    
        for(int j = 0; j < 2; j++)
	{ 
            QuadEdge::Simplex * simplex = \
                const_cast<QuadEdge::Simplex *>(edge->GetOrigin());
         
            if( (simplex != 0) && (simplex->FirstEdge(0) == edge) )
	    {
                // add it 
                simplices.push_back(simplex);
	    }
         
            edge = edge->Sym(); 
	}
    }
 
    return 1;
}

int Mesh::GetFaces(vector<QuadEdge::Simplex *> & simplices) const
{
    simplices.clear();

    for(unsigned int i = 0; i < _edges.size(); i++)
    {
        const QuadEdge::Edge * edge = _edges[i]->InvRot();

        for(int j = 0; j < 2; j++)
	{
            QuadEdge::Simplex * simplex = \
                const_cast<QuadEdge::Simplex *>(edge->GetOrigin());

            if( (simplex != 0) && (simplex->FirstEdge(0) == edge) )
	    {
                // add it 
                simplices.push_back(simplex);
	    }

            edge = edge->Sym();
	}
    }

    return 1;
}

int Mesh::InsertEdge(QuadEdge::Edge & edge)
{
    _edges.push_back(reinterpret_cast<QuadEdge::Edge *>(edge.QEdge()));

    return 1;
}

int Mesh::InsertFace(const vector<QuadEdge::Simplex *> & vertices,
		     QuadEdge::Simplex * face)
{
    if(vertices.size() <= 0)
    {
      return 0;
    }

    QuadEdge::Edge * pReferenceEdge = 0;

    for(unsigned int i = 0; i < vertices.size(); i++)
    {
      if(vertices[i] == 0)
      {
            return 0;
      }
    }

    // check to see if this face can not be inserted
    // due to orientation conflicting with existing faces
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        QuadEdge::Simplex & v1 = *vertices[i];
        QuadEdge::Simplex & v2 = *vertices[(i+1) % vertices.size()];

        const QuadEdge::Edge * edge = FindEdge(v1, v2);

        // check if this edge exist
        if( (edge != 0) && \
            (edge == reinterpret_cast<QuadEdge::Edge *>(edge->QEdge())) )
        {
            return 0;
        }       
    }

    // edge list
    vector<QuadEdge::Edge *> edgeList;

    // create the edges, if not already exist
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        QuadEdge::Simplex & v1 = *vertices[i];
        QuadEdge::Simplex & v2 = *vertices[(i+1) % vertices.size()];

        // check if this edge exist
        pReferenceEdge = const_cast<QuadEdge::Edge *>(FindEdge(v1, v2));

        // if edge does not exist, create a new one
        if( pReferenceEdge == 0)
        {
            // insert this edge
            if( (pReferenceEdge = QuadEdge::MakeEdge()) == 0 )
            {
                return 0;
            }

            InsertEdge(*pReferenceEdge);
        }
   
        // put into the edgeList for this face
        edgeList.push_back(pReferenceEdge);
    }

    // do the proper connections (splice)
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        pReferenceEdge = edgeList[i];
     
        // find the previous edge on the face (in CCW order)
        QuadEdge::Edge * pPreviousEdge = \
            edgeList[(i+edgeList.size()-1)%edgeList.size()];

        if(pPreviousEdge != 0)
	{
            // do the proper splice operations
            //QuadEdge::Splice(pReferenceEdge, pPreviousEdge->Lnext());
            QuadEdge::Connect(pReferenceEdge, pPreviousEdge->Lnext());
	}
        else
	{
            // error
            return 0;
	}
    }

    // do the proper simplex assignment
    for(unsigned int i = 0; i < vertices.size(); i++)
    {
        QuadEdge::Simplex & v1 = *vertices[i];
        QuadEdge::Simplex & v2 = *vertices[(i+1) % vertices.size()];

        pReferenceEdge = edgeList[i];
      
        if( (!pReferenceEdge->SetOrigin(&v1)) || \
            (!pReferenceEdge->SetDestination(&v2)) )
        {
            return 0;
        }

        if(!pReferenceEdge->SetLeft(face))
        {
            return 0;
        } 
    }

    return 1;
}

const QuadEdge::Edge * Mesh::FindEdge(const QuadEdge::Simplex & s1,
				      const QuadEdge::Simplex & s2) const
{
    const QuadEdge::Edge * pAnswerEdge = 0;

    for(int i = 0; i < s1.NumEdgeRings(); i++)
    {
        const QuadEdge::Edge * candidate = s1.FirstEdge(i);
      
        do
	{
            candidate = candidate->Onext();
	  
            if( (candidate->GetDestination()) == (&s2) )
	    {
                pAnswerEdge = candidate;
                break;
	    }
	}
        while( (candidate != s1.FirstEdge(i)) );
    }
    
    return pAnswerEdge;
}

Mesh::Mesh(const Mesh & rhs)
{
    cerr<<"Mesh::Mesh(const Mesh & rhs) called"<<endl;
    throw Exception();
}

Mesh & Mesh::operator=(const Mesh & rhs)
{
    cerr<<"Mesh::operator=(const Mesh & rhs) called"<<endl;
    throw Exception();
}

