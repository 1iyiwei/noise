/*
 * QuadEdge.hpp
 *
 * the quad edge data structure for orientable 2-manifolds
 * adaped from Dani Lischinski's Graphics Gems code for
 * Constrained Delaunay Triangulation
 *
 * Li-Yi Wei
 * 9/7/00
 *
 */

#ifndef _QUAD_EDGE_HPP
#define _QUAD_EDGE_HPP

#include <vector>
using namespace std;

class QuadEdge 
{
    friend class Mesh; // for direct deletion

public:
    class Exception {};
    class Edge;
    
    class Simplex
    {
        friend Edge;
        friend class Mesh; // for direct deletion
    public:
        typedef unsigned char MarkType;
        class Exception {};

    public:
        Simplex(void);
        Simplex(const int id);
        
        // ID
        int & ID(void);
        const int & ID(void) const;

        // first edge of a ring originating from this simplex
        const Edge * FirstEdge(const int whichRing) const;

        // number of edge rings originating from this simplex
        int NumEdgeRings(void) const;

        // Mark, assumed to be 0 in stable state between operations
        // return the mark value
        MarkType GetMark(void) const;
        // set the mark value
        void SetMark(const MarkType newMark) const;

        // delete this simplex
        // this is the only way accessible for deletion
        // return 1 if successful, 0 else
        static int Delete(Simplex * simplex);

    protected:
        virtual ~Simplex(void) = 0;

        // add or remove an edge from the list of edges
        // originating from  this simplex
        // will return the new pointer to this simplex
        // it will get deleted if all edges are removed
        Simplex * AddEdge(Edge * edge);
        Simplex * RemoveEdge(Edge * edge);
        
        // return the entry of _edges which contains edge in the rings
        // return -1 if not found, otherwise will return the entry number
        int FindEdge(const Edge * edge) const;
        // similar to above, except that we only search [startIndex, endIndex]
        // of _edges
        // no error checking for the ranges
        int FindEdge(const Edge * edge, const int startIndex, const int endIndex) const;
        // merge the entries of _edges which are of the same ring
        void MergeEdges(void) const;

    private:
        // no copy or assignment
        Simplex(const Simplex & rhs);
        Simplex & operator=(const Simplex & rhs);

    protected:
        // an id that's unique among vertices or faces of a mesh
        int _id;

        // mark, for various algorithmic purposes 
        // such as patch flattening
        mutable MarkType _mark;

        // a list of pointers to the first edge of edge rings 
        // originating from this simplex
        mutable vector<Edge *> _edges;
    };

    class Edge 
    {
        friend QuadEdge;
   
    protected:
        Edge(void);
        virtual ~Edge(void);
    
    public:
        typedef unsigned char MarkType;

        // Euler operations
        Edge * Rot(void);
        Edge * InvRot(void);
        Edge * Sym(void);
        Edge * Onext(void);
        Edge * Oprev(void);
        Edge * Dnext(void);
        Edge * Dprev(void);
        Edge * Lnext(void);	
        Edge * Lprev(void);
        Edge * Rnext(void);	
        Edge * Rprev(void);
    
        const Edge * Rot(void) const;
        const Edge * InvRot(void) const;
        const Edge * Sym(void) const;
        const Edge * Onext(void) const;
        const Edge * Oprev(void) const;
        const Edge * Dnext(void) const;
        const Edge * Dprev(void) const;
        const Edge * Lnext(void) const;	
        const Edge * Lprev(void) const;
        const Edge * Rnext(void) const;	
        const Edge * Rprev(void) const;
       
        // the containing quad edge structure
        QuadEdge * QEdge(void) const;
    
        // handle the Simplex
        const Simplex * GetOrigin(void) const;
        int SetOrigin(Simplex * original);

        const Simplex * GetDestination(void) const;
        int SetDestination(Simplex * original);

        const Simplex * GetLeft(void) const;
        int SetLeft(Simplex * original);

        const Simplex * GetRight(void) const;
        int SetRight(Simplex * original);

        // Mark, assumed to be 0 in stable state between operations
        // return the mark value
        MarkType GetMark(void) const;
        // set the mark value
        void SetMark(const MarkType newMark) const;

    private:
        // no copy or assignment
        Edge(const Edge & rhs);
        Edge & operator=(const Edge & rhs);

    protected:
        char _num; 
        mutable MarkType _mark;
        Edge * _next;
        // the source simplex (either a vertex, or a face)
        Simplex * _simplex;
    };

private:
    // put here so that the individual edges can access the pointer to
    // the containing QuadEdge
    Edge _e[4];

public:
    QuadEdge(void);
 
    static Edge * MakeEdge(void);

    // if e1 and e2 are not of the same ring, call Splice(e1, e2)
    // else do nothing
    static int Connect(Edge * e1, Edge * e2);

    // delete edge e and its destination (if different from source)
    static void DeleteVertexEdge(Edge * e);
    // delete edge e and its right (if different from left)
    static void DeleteFaceEdge(Edge * e);
 
protected:
  
    // This operator affects the two edge rings around the origins of a and b,
    // and, independently, the two edge rings around the left faces of a and b.
    // In each case,
    // (i) if the two rings are distinct,
    // Splice will combine them into one;
    // (ii) if the two are the same ring,
    // Splice will break it into two separate pieces. 
    // Thus, Splice can be used both to attach the two edges together,
    // and to break them apart.
    // See Guibas and Stolfi (1985) p.96 for more details and illustrations.
    static int Splice(Edge * e1, Edge * e2);

protected:
    // delete using DeleteEdge
    virtual ~QuadEdge(void);  
};

/************************* Implementation ***********************************/

inline int & QuadEdge::Simplex::ID(void)
{
    return _id;
}

inline const int & QuadEdge::Simplex::ID(void) const
{
    return _id;
}

inline QuadEdge::Simplex::MarkType QuadEdge::Simplex::GetMark(void) const
{
    return _mark;
}

inline void QuadEdge::Simplex::SetMark(const QuadEdge::Simplex::MarkType newMark) const
{
    _mark = newMark;
}

inline const QuadEdge::Edge * QuadEdge::Simplex::FirstEdge(const int whichRing) const
{
    if(_edges.size() > 1) 
    {
        MergeEdges();  
    }

    if( (whichRing >= 0) && (whichRing < static_cast<int>(_edges.size())) )
    {
        return _edges[whichRing];
    }
    else
    {
        return 0;
    }

    /*
      MergeEdges();
 
      if(_edges.size() > 0)
      {
      return _edges[0];
      }
      else
      {
      return 0;
      }
    */
}

/************************* Edge Algebra *************************************/

inline QuadEdge::Edge * QuadEdge::Edge::Rot(void)
// Return the dual of the current edge, directed from its right to its left. 
{
    //return (num < 3) ? this + 1 : this - 3;
    return ( this - _num + ((_num+1)%4) );
}

inline QuadEdge::Edge * QuadEdge::Edge::InvRot(void)
// Return the dual of the current edge, directed from its left to its right. 
{
    //return (num > 0) ? this - 1 : this + 3;
    return ( this - _num + ((_num+3)%4) );
}

inline QuadEdge::Edge * QuadEdge::Edge::Sym(void)
// Return the edge from the destination to the origin of the current edge.
{
    //return (num < 2) ? this + 2 : this - 2;
    return ( this - _num + ((_num+2)%4) );
}
	
inline QuadEdge::Edge * QuadEdge::Edge::Onext(void)
// Return the next ccw edge around (from) the origin of the current edge.
{
    //return next;
    return _next;
}

inline QuadEdge::Edge * QuadEdge::Edge::Oprev(void)
// Return the next cw edge around (from) the origin of the current edge.
{
    return Rot()->Onext()->Rot();
    //return Rot().Onext().Rot();
}

inline QuadEdge::Edge * QuadEdge::Edge::Dnext(void)
// Return the next ccw edge around (into) the destination of the current edge.
{
    return Sym()->Onext()->Sym();
    //return Sym().Onext().Sym();
}

inline QuadEdge::Edge * QuadEdge::Edge::Dprev(void)
// Return the next cw edge around (into) the destination of the current edge.
{
    return InvRot()->Onext()->InvRot();
    //return InvRot().Onext().InvRot();
}

inline QuadEdge::Edge * QuadEdge::Edge::Lnext(void)
// Return the ccw edge around the left face following the current edge.
{
    return InvRot()->Onext()->Rot();
    //return InvRot().Onext().Rot();
}

inline QuadEdge::Edge * QuadEdge::Edge::Lprev(void)
// Return the ccw edge around the left face before the current edge.
{
    return Onext()->Sym();
    //return Onext().Sym();
}

inline QuadEdge::Edge * QuadEdge::Edge::Rnext(void)
// Return the edge around the right face ccw following the current edge.
{
    return Rot()->Onext()->InvRot();
    //return Rot().Onext().InvRot();
}

inline QuadEdge::Edge * QuadEdge::Edge::Rprev(void)
// Return the edge around the right face ccw before the current edge.
{
    return Sym()->Onext();
    //return Sym().Onext();
}

/******************** const Edge Algebra *************************************/

inline const QuadEdge::Edge * QuadEdge::Edge::Rot(void) const
// Return the dual of the current edge, directed from its right to its left. 
{
    //return (num < 3) ? this + 1 : this - 3;
    return ( this - _num + ((_num+1)%4) );
}

inline const QuadEdge::Edge * QuadEdge::Edge::InvRot(void) const
// Return the dual of the current edge, directed from its left to its right. 
{
    //return (num > 0) ? this - 1 : this + 3;
    return ( this - _num + ((_num+3)%4) );
}

inline const QuadEdge::Edge * QuadEdge::Edge::Sym(void) const
// Return the edge from the destination to the origin of the current edge.
{
    //return (num < 2) ? this + 2 : this - 2;
    return ( this - _num + ((_num+2)%4) );
}
	
inline const QuadEdge::Edge * QuadEdge::Edge::Onext(void) const
// Return the next ccw edge around (from) the origin of the current edge.
{
    //return next;
    return _next;
}

inline const QuadEdge::Edge * QuadEdge::Edge::Oprev(void) const
// Return the next cw edge around (from) the origin of the current edge.
{
    return Rot()->Onext()->Rot();
    //return Rot().Onext().Rot();
}

inline const QuadEdge::Edge * QuadEdge::Edge::Dnext(void) const
// Return the next ccw edge around (into) the destination of the current edge.
{
    return Sym()->Onext()->Sym();
    //return Sym().Onext().Sym();
}

inline const QuadEdge::Edge * QuadEdge::Edge::Dprev(void) const
// Return the next cw edge around (into) the destination of the current edge.
{
    return InvRot()->Onext()->InvRot();
    //return InvRot().Onext().InvRot();
}

inline const QuadEdge::Edge * QuadEdge::Edge::Lnext(void) const
// Return the ccw edge around the left face following the current edge.
{
    return InvRot()->Onext()->Rot();
    //return InvRot().Onext().Rot();
}

inline const QuadEdge::Edge * QuadEdge::Edge::Lprev(void) const
// Return the ccw edge around the left face before the current edge.
{
    return Onext()->Sym();
    //return Onext().Sym();
}

inline const QuadEdge::Edge * QuadEdge::Edge::Rnext(void) const
// Return the edge around the right face ccw following the current edge.
{
    return Rot()->Onext()->InvRot();
    //return Rot().Onext().InvRot();
}

inline const QuadEdge::Edge * QuadEdge::Edge::Rprev(void) const
// Return the edge around the right face ccw before the current edge.
{
    return Sym()->Onext();
    //return Sym().Onext();
}


inline QuadEdge::Edge::MarkType QuadEdge::Edge::GetMark(void) const
{
    return _mark;
}

inline void QuadEdge::Edge::SetMark(const QuadEdge::Edge::MarkType newMark) const
{
    _mark = newMark;
}

#endif
