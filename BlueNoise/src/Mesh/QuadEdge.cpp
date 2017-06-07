/*
 * QuadEdge.cpp
 *
 * Li-Yi Wei
 * 9/8/00
 * 10/30/2007 modified
 */

#include <iostream>
using namespace std;

#include <stdlib.h>
#include "QuadEdge.hpp"

QuadEdge::Simplex::Simplex(void) : _id(-1), _mark(0), _edges(0)
{ 
    // nothing to do
}

QuadEdge::Simplex::Simplex(const int id) : _id(id), _mark(0), _edges(0)
{ 
    // nothing to do
}

QuadEdge::Simplex::~Simplex(void) 
{ 
    // nothing to do
    // will be handled by Delete
}

int QuadEdge::Simplex::NumEdgeRings(void) const
{
    return _edges.size();
}

QuadEdge::Simplex * QuadEdge::Simplex::AddEdge(Edge * edge)
{
    MergeEdges();
 
    if(edge != 0)
    {
        int whichOne = FindEdge(edge); 
        if(whichOne < 0)
	{
            _edges.push_back(edge);
	}
        else
	{
            _edges[whichOne] = edge;
	}
    }

    return this;
}

QuadEdge::Simplex * QuadEdge::Simplex::RemoveEdge(Edge * edge)
{ 
    MergeEdges();

/*
    if(_edges.size() > 1)
    {
        // delete splitted simplex
        for(unsigned int i = 0; i < _edges.size(); i++)
        {
            QuadEdge::Edge * myEdge = _edges[i];

            do
            {
                myEdge = myEdge->Onext();
                // can not call myEdge->SetOrigin() to avoid endless recursions
                myEdge->_simplex = 0; 
	    }
            while(myEdge != _edges[i]);
	}

        _edges.clear();
        Delete(this);
        return 0;
    }
*/

    int whichOne = FindEdge(edge);
    if(whichOne < 0)
    { 
        // do nothing
        return this;
    }
    else
    {
        _edges[whichOne] = _edges[_edges.size()-1];
        _edges.pop_back();
      
        if( (edge->Oprev()->QEdge() != edge->QEdge()) &&
            (edge->Oprev()->GetOrigin() == this) )
	{
            _edges.push_back(edge->Oprev());
	}
        
        if( (edge->Onext()->QEdge() != edge->QEdge()) &&
            (edge->Onext()->GetOrigin() == this) )
	{
            _edges.push_back(edge->Onext());
	}       
      
      /*
      if( (edge->Oprev()->QEdge() != edge->QEdge()) &&
	  (edge->Onext()->QEdge() != edge->QEdge()) )
	{
	  _edges[whichOne] = edge->Oprev();
	  _edges.push_back(edge->Onext());
	}
      else if(_edges[whichOne]->Onext()->QEdge() != _edges[whichOne]->QEdge())
	{
	  _edges[whichOne] = _edges[whichOne]->Onext();
	}
      else
	{
	  _edges[whichOne] = _edges[_edges.size()-1];
	  _edges.pop_back();
	}
      */

        if(_edges.size() <= 0)
	{
            Delete(this);
            return 0;
	}
        else
	{
            return this;
	}
    }
}

int QuadEdge::Simplex::FindEdge(const Edge * queryEdge) const
{
    return FindEdge(queryEdge, 0, _edges.size()-1);
}

int QuadEdge::Simplex::FindEdge(const Edge * queryEdge, 
				const int startIndexInput, 
				const int endIndexInput) const
{
    int startIndex = startIndexInput;
    int endIndex = endIndexInput;

    if(endIndex >= (static_cast<int>(_edges.size())-1))
    {
        endIndex = _edges.size()-1;
    }

    for(int i = startIndex; i <= endIndex; i++)
    {
        const QuadEdge::Edge * edge = _edges[i];

        do
	{
            edge = edge->Onext();
            if(edge == queryEdge)
	    {
                return i;
	    }
	}
        while(edge != _edges[i]);
    }

    return -1;
}

void QuadEdge::Simplex::MergeEdges(void) const
{
    if(_edges.size() <= 1)
    {
        return;
    }

    int changed = 1;
  
    while(changed)
    {
        changed = 0;

        for(unsigned int i = 0; i < _edges.size(); i++)
	{
            //int result = FindEdge(_edges[i]); 
            int result = FindEdge(_edges[i], \
                                  i+1, static_cast<int>(_edges.size())-1);
            //if((result >= 0) && (result != static_cast<int>(i)))
            if(result >= 0)
	    {
                _edges[i] = _edges[_edges.size()-1];
                _edges.pop_back();
                changed = 1;
                break;
	    }
	}
    }
}

#include "Vertex.hpp"
#include "Face.hpp"
int QuadEdge::Simplex::Delete(Simplex * simplex)
{
    // set the source pointers of all out going edges to 0
 
    if(simplex->NumEdgeRings() > 0)
    {
        // can not be deleted
        return 0;
    }
    else
    {
        delete simplex;
        return 1;
    }
}

QuadEdge::Simplex::Simplex(const QuadEdge::Simplex & rhs)
{
    cerr<<"QuadEdge::Simplex::Simplex(const QuadEdge::Simplex & rhs) called"<<endl;
    throw Exception();
}

QuadEdge::Simplex & QuadEdge::Simplex::operator=(const QuadEdge::Simplex & rhs)
{
    cerr<<"QuadEdge::Simplex & operator=(const QuadEdge::Simplex & rhs) called"<<endl;
    throw QuadEdge::Exception();
}

QuadEdge::Edge::Edge(void) : _num(0), _mark(0), _next(0), _simplex(0)
{
    // nothing to do
}

QuadEdge::Edge::~Edge(void)
{
    // do nothing
    // simplices will be deleted by the containing QuadEdge class
}

// origin
const QuadEdge::Simplex * QuadEdge::Edge::GetOrigin(void) const
{
    return _simplex;
}

int QuadEdge::Edge::SetOrigin(QuadEdge::Simplex * original)
{
    if(_simplex == original)
    {
        // do nothing
        return 1;
    }
    else
    {
        if(_simplex != 0)
	{
            // handle the edge pointer of the original source simplex
            _simplex->RemoveEdge(this);
	}

        _simplex = original;
      
        // think about this
        if(_simplex != 0)
	{
            _simplex->AddEdge(this);
	}
    }
  
    return 1;
}

// destination
const QuadEdge::Simplex * QuadEdge::Edge::GetDestination(void) const
{
    return Sym()->GetOrigin();
}

int QuadEdge::Edge::SetDestination(QuadEdge::Simplex * original)
{
    return Sym()->SetOrigin(original);
}

//left
const QuadEdge::Simplex * QuadEdge::Edge::GetLeft(void) const
{
    return InvRot()->GetOrigin();
}

int QuadEdge::Edge::SetLeft(QuadEdge::Simplex * original)
{
    return InvRot()->SetOrigin(original);
}

//right
const QuadEdge::Simplex * QuadEdge::Edge::GetRight(void) const
{
    return Rot()->GetOrigin();
}

int QuadEdge::Edge::SetRight(QuadEdge::Simplex * original)
{
    return Rot()->SetOrigin(original);
}

QuadEdge::Edge::Edge(const QuadEdge::Edge & rhs)
{
    cerr<<"QuadEdge::Edge::Edge(const QuadEdge::Edge & rhs) called"<<endl;
    throw Exception();
}

QuadEdge::Edge & QuadEdge::Edge::operator=(const QuadEdge::Edge & rhs)
{
    cerr<<"QuadEdge::Edge & operator=(const QuadEdge::Edge & rhs) called"<<endl;
    throw QuadEdge::Exception();
}

QuadEdge::QuadEdge(void)
{
    _e[0]._num = 0, _e[1]._num = 1, _e[2]._num = 2, _e[3]._num = 3;
    _e[0]._next = &(_e[0]); _e[1]._next = &(_e[3]);
    _e[2]._next = &(_e[2]); _e[3]._next = &(_e[1]);
}

QuadEdge::~QuadEdge(void)
{
    // nothing to do
    // all maintaining operations are in DeleteVertexEdge and DeleteFaceEdge
}

QuadEdge * QuadEdge::Edge::QEdge(void) const
{
    return (QuadEdge *)(this - _num);
}

QuadEdge::Edge * QuadEdge::MakeEdge(void)
{
    QuadEdge * quadEdge = new QuadEdge;
    return (& (quadEdge->_e[0]));
}

int QuadEdge::Connect(Edge * a, Edge * b)
{
    if( (a == 0) || (b == 0) )
    {
        return 0;
    }

    int connected = 0;

    QuadEdge::Edge *edge = a;

    do
    {
        edge = edge->Onext();
      
        if(edge == b)
	{
            connected = 1;
            break;
	}
    }
    while(edge != a);

    if(! connected )
    {
        return Splice(a,b);
    }
    else
    {
        // already connected
        return 1;
    }
}

int QuadEdge::Splice(Edge * a, Edge * b)
{
    if( (a == 0) || (b == 0) )
    {
        return 0;
    }

    Edge * alpha = a->Onext()->Rot();
    Edge * beta = b->Onext()->Rot();

    Edge * t1 = b->Onext();
    Edge * t2 = a->Onext();
    Edge * t3 = beta->Onext();
    Edge * t4 = alpha->Onext();

    a->_next = t1;
    b->_next = t2;
    alpha->_next = t3;
    beta->_next = t4;

    return 1;
}

void QuadEdge::DeleteVertexEdge(Edge * e)
{
    DeleteFaceEdge(e->InvRot());
}

void QuadEdge::DeleteFaceEdge(Edge * e)
{ 
    if(e == 0)
    {
        return;
    }

    /*
      cout<<"delete edge :";
      if(e->GetOrigin() != 0) cout<<"origin "<<e->GetOrigin()->ID()<<", ";
      if(e->GetDestination() != 0) cout<<"destination "<<e->GetDestination()->ID()<<", ";
      if(e->GetLeft() != 0) cout<<"left "<<e->GetLeft()->ID()<<", ";
      if(e->GetRight() != 0) cout<<"right "<<e->GetRight()->ID()<<", ";
      cout<<endl;
    */

    // delete right face
    // by pointing the source of all the right face edges to e->Left
    QuadEdge::Edge * edge = e->Sym();
    QuadEdge::Simplex * face = const_cast<QuadEdge::Simplex *>(e->GetLeft());
    
    do
    {
        edge = edge->Lnext();
        edge->SetLeft(face);
    }
    while(edge != e->Sym());

    e->SetLeft(0);
    e->SetRight(0);

    // handle the source and destination pointers
    e->SetOrigin(0);
    e->SetDestination(0);
  
    // topological operations
    Splice(e, e->Oprev());
    Splice(e->Sym(), e->Sym()->Oprev());

    // the actual deletion of the data structure
    delete e->QEdge();
}
