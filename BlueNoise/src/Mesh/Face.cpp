/*
 * Face.cpp
 *
 * Li-Yi Wei
 * 9/15/00
 * 10/30/07 modified
 */

#include "Face.hpp"

Face::Face(void) : _nextEdge(0)
{
    // nothing to do
}

Face::~Face(void)
{ 
    // nothing to do
}

void Face::ResetEdgeIterator(const int whichRing) const
{
    // _nextEdge = FirstEdge()->Rot();
    if(_edges.size() > 1)
    {
        MergeEdges();
    }

    _nextEdge = FirstEdge(whichRing)->Rot();
    _whichRing = whichRing;
}

const QuadEdge::Edge * Face::NextEdge(void) const
{
    const QuadEdge::Edge * answer = _nextEdge;

    if(_nextEdge == 0)
    {
        // just return
    }
    else
    {
        _nextEdge = _nextEdge->Lnext();
      
        if(_nextEdge == FirstEdge(_whichRing)->Rot())
	{
            _nextEdge = 0;
	}
    }

    return answer;
}
