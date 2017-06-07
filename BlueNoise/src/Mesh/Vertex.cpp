/*
 * Vertex.cpp
 *
 * Li-Yi Wei
 * 9/15/00
 * 10/30/07 modified
 */

#include "Vertex.hpp"

Vertex::Vertex(void) : _nextEdge(0)
{
    // nothing to do
}

Vertex::~Vertex(void)
{ 
    // nothing to do
}

void Vertex::ResetEdgeIterator(const int whichRing) const
{
    //_nextEdge = FirstEdge();
    MergeEdges();
    _nextEdge = FirstEdge(whichRing);
    _whichRing = whichRing;
}

const QuadEdge::Edge * Vertex::NextEdge(void) const
{
    const QuadEdge::Edge * answer = _nextEdge;

    if(_nextEdge == 0)
    {
        // just return
    }
    else
    {
        _nextEdge = _nextEdge->Onext();
      
        if(_nextEdge == FirstEdge(_whichRing))
	{
            _nextEdge = 0;
	}
    }

    return answer;
}
