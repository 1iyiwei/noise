/*
  Delaunay.hpp

  transformed from Subidivion class in Lischinski's original code
  but with better software engineering and decouple of vertices

  Li-Yi Wei
  10/31/2007

*/

#ifndef _DELAUNAY_HPP
#define _DELAUNAY_HPP

#include "QuadEdge.hpp"

class Delaunay
{
public:
    typedef double Real;

public:
    class Point2D;

    class Vector2D
    {
    public:
	Vector2D(void);
        Vector2D(const Point2D & point);
	Vector2D(const Real x, const Real y);

        Real norm(void) const;
	void normalize(void);

        Vector2D operator+(const Vector2D & v) const;
	Vector2D operator-(const Vector2D & v) const;
        
	static Real dot(const Vector2D & a, const Vector2D & b);
        
	Real x, y;
    };

public:
    class Point2D : public QuadEdge::Simplex
    {
    public:
        typedef enum {ANCHOR, FREE, ORPHAN} Category;
        
        Point2D(void);
        Point2D(const Vector2D & vector);
        Point2D(const Real x, const Real y);
        Point2D(const Real x, const Real y, const Category category);
        virtual ~Point2D(void);

        virtual Point2D * Clone(void) const;
        
	Point2D operator+(const Vector2D &) const;
	Vector2D operator-(const Point2D &) const;
	// int operator==(const Point2D &) const;
        bool Equal(const Point2D &, const Real very_small) const;

        Real x, y;

        Category category;

        int id;
    };

protected:
    class Line
    {
    public:
	Line(void);
	Line(const Point2D & a, const Point2D & b);
	Real eval(const Point2D & p) const;
	int classify(const Point2D & p, const Real very_small) const;
        
    private:
	Real _a, _b, _c;
    };

public:
    class AABB
    {
    public:
        AABB(void);
        AABB(Real x_min, const Real x_max, const Real y_min, const Real y_max);

        bool Inside(const Point2D &p) const;

        // private:
        Real x_min;
        Real x_max;
        Real y_min;
        Real y_max;
    };

public:
    // initial triangle big enough to contain all points
    Delaunay(const Point2D & a, const Point2D & b, const Point2D & c);
    // specify initial bounding rectangle
    Delaunay(const Real x_min, const Real x_max, const Real y_min, const Real y_max); 
    Delaunay(const Point2D & a, const Point2D & b, const Point2D & c, const Real very_small);
    // specify initial bounding rectangle
    Delaunay(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small);
    ~Delaunay(void);
    
    void GetEdges(vector<const QuadEdge::Edge * > & edges) const;
    
    bool NearestSite(const Point2D & query, Point2D & result) const;

    enum InsertStatus {INSERT_FAIL, INSERT_OK, INSERT_DUPLICATE};

    InsertStatus InsertSite(Point2D & newbie);

protected:
    void Construct(const Point2D & a, const Point2D & b, const Point2D & c);
    
    void Construct(const Real x_min, const Real x_max, const Real y_min, const Real y_max);

protected:
    QuadEdge::Edge * Locate(const Point2D & query) const;

    // Add a new edge e connecting the destination of a to the
    // origin of b, in such a way that all three have the same
    // left face after the connection is complete.
    QuadEdge::Edge * Connect(QuadEdge::Edge* a, QuadEdge::Edge* b);
    
    // Essentially turns edge e counterclockwise inside its enclosing
    // quadrilateral. The data pointers are modified accordingly.
    void Swap(QuadEdge::Edge* e);

    // some geometry stuff

    // Returns twice the area of the oriented triangle (a, b, c), i.e., the
    // area is positive if the triangle is oriented counterclockwise.
    static Real TriArea(const Point2D & a, const Point2D & b, const Point2D & c);
    
    // Returns TRUE if the point d is inside the circle defined by the
    // points a, b, c. See Guibas and Stolfi (1985) p.107.
    static int InCircle(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d);

    // Returns TRUE if the points a, b, c are in a counterclockwise order
    static int ccw(const Point2D& a, const Point2D& b, const Point2D& c);

    static int RightOf(const Point2D & x, const QuadEdge::Edge * e);
    static int LeftOf(const Point2D & x, const QuadEdge::Edge * e);

    int OnEdge(const Point2D & x, const QuadEdge::Edge * e) const;

public:
    // convenience functions
    static const Point2D & GetOrigin(const QuadEdge::Edge * e);
    static const Point2D & GetDestination(const QuadEdge::Edge * e);
    static Point2D & GetOrigin(QuadEdge::Edge * e);
    static Point2D & GetDestination(QuadEdge::Edge * e);

protected:
    // call these functions instead of
    // QuadEdge::MakeEdge() and QuadEdge::DeleteEdge()
    QuadEdge::Edge * MakeEdge(void);
    void DeleteEdge(QuadEdge::Edge * victim);
    
    void CheckEdgeConsistency(const QuadEdge::Edge * edge) const;
    
protected:
    mutable QuadEdge::Edge * _starting_edge;     
    vector<const Point2D *> _anchors;
    vector<QuadEdge::Edge * > _edges;

    static const Real _VERY_SMALL;
    const Real _very_small;
};

Delaunay::Vector2D operator*(Delaunay::Real c, const Delaunay::Vector2D & v);

#endif

