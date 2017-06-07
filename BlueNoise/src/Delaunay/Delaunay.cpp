/*
  Delaunay.cpp

  Li-Yi Wei
  10/31/2007

*/

#include "Delaunay.hpp"
#include "Exception.hpp"

#include <math.h>
#include <assert.h>

Delaunay::Delaunay(const Point2D & a, const Point2D & b, const Point2D & c) : _starting_edge(0)
{
    Point2D *da, *db, *dc;
    da = new Point2D(a), db = new Point2D(b), dc = new Point2D(c);
    da->category = db->category = dc->category = Point2D::ANCHOR;
    _anchors.push_back(da); _anchors.push_back(db); _anchors.push_back(dc);
    
    QuadEdge::Edge * ea = MakeEdge();
    ea->SetEndPoints(da, db);
        
    QuadEdge::Edge * eb = MakeEdge();
    QuadEdge::Splice(ea->Sym(), eb);
    eb->SetEndPoints(db, dc);

    QuadEdge::Edge * ec = MakeEdge();
    QuadEdge::Splice(eb->Sym(), ec);
    ec->SetEndPoints(dc, da);
    QuadEdge::Splice(ec->Sym(), ea);
    
    _starting_edge = ea;
}

Delaunay::Delaunay(const Real x_min, const Real x_max, const Real y_min, const Real y_max) : _starting_edge(0)
{
    Point2D *d0, *d1, *d2, *d3;
    d0 = new Point2D(x_min, y_min, Point2D::ANCHOR);
    d1 = new Point2D(x_max, y_min, Point2D::ANCHOR);
    d2 = new Point2D(x_max, y_max, Point2D::ANCHOR);
    d3 = new Point2D(x_min, y_max, Point2D::ANCHOR);
    _anchors.push_back(d0);
    _anchors.push_back(d1);
    _anchors.push_back(d2);
    _anchors.push_back(d3);
    
    QuadEdge::Edge * e0 = MakeEdge();
    e0->SetEndPoints(d0, d1);
        
    QuadEdge::Edge * e1 = MakeEdge();
    e1->SetEndPoints(d1, d2);
    QuadEdge::Splice(e0->Sym(), e1);

    QuadEdge::Edge * e2 = MakeEdge();
    e2->SetEndPoints(d2, d0);
    QuadEdge::Splice(e1->Sym(), e2);
    QuadEdge::Splice(e2->Sym(), e0);
    
    QuadEdge::Edge * e3 = MakeEdge();
    e3->SetEndPoints(d3, d0);
    //QuadEdge::Splice(e3->Sym(), e0);
    QuadEdge::Splice(e3->Sym(), e2->Sym());
    
    QuadEdge::Edge * e4 = MakeEdge();
    e4->SetEndPoints(d3, d2);
    QuadEdge::Splice(e3, e4);
    //QuadEdge::Splice(e2, e4->Sym());
    QuadEdge::Splice(e1->Sym(), e4->Sym());
    
    _starting_edge = e0;
}

Delaunay::~Delaunay(void)
{
    for(unsigned int i = 0; i < _edges.size(); i++)
    {
        CheckEdgeConsistency(_edges[i]);
    }
    
    for(unsigned int i = 0; i < _edges.size(); i++)
    {
        QuadEdge::DeleteEdge(_edges[i]);
    }
    
    for(unsigned int i = 0; i < _anchors.size(); i++)
    {
        delete _anchors[i];
    }
}

void Delaunay::GetEdges(vector<const QuadEdge::Edge * > & edges) const
{
    edges.clear();

    for(unsigned int i = 0; i < _edges.size(); i++)
    {
        edges.push_back(_edges[i]);
    }
}

int Delaunay::InsertSite(Point2D & x)
{
    QuadEdge::Edge* e = Locate(x);

    if(e == 0)
    {
        return 0;
    }

    if((x == GetOrigin(e)) || (x == GetDestination(e)))
    {
        // point is already in
        return 1;
    }
    else if(OnEdge(x, e))
    {
        e = e->Oprev();
        DeleteEdge(e->Onext());
    }

    // Connect the new point to the vertices of the containing triangle
    // (or quadrilateral, if the new point fell on anexisting edge.)
	QuadEdge::Edge* base = MakeEdge();
    base->SetEndPoints(&GetOrigin(e), &x);
    QuadEdge::Splice(base, e);

    _starting_edge = base;
    do
    {
        base = Connect(e, base->Sym());
        e = base->Oprev();
    }
    while(e->Lnext() != _starting_edge);

    // Examine suspect edges to ensure that the Delaunay condition is satisfied
    do
    {
        QuadEdge::Edge* t = e->Oprev();
        if(RightOf(GetDestination(t), e) && InCircle(GetOrigin(e), GetDestination(t), GetDestination(e), x))
        {
            Swap(e);
            e = e->Oprev();
        }
        else if (e->Onext() == _starting_edge)
        {
            // no more suspect edges
            return 1;
        }
        else
        {
            // pop a suspect edge
            e = e->Onext()->Lprev();
        }
    }
    while(1);

    throw Exception("this should never be reached");
    return 0;
}

QuadEdge::Edge * Delaunay::Locate(const Point2D & x)
{
    QuadEdge::Edge* e = _starting_edge;

    while(1)
    {
        if(x == GetOrigin(e) || x == GetDestination(e))
            return e;
        else if (RightOf(x, e))
            e = e->Sym();
        else if (!RightOf(x, e->Onext()))
            e = e->Onext();
        else if (!RightOf(x, e->Dprev()))
            e = e->Dprev();
        else
            return e;
    }
    
    throw Exception("this should never be reached");
    return 0;     
}

QuadEdge::Edge * Delaunay::Connect(QuadEdge::Edge* a, QuadEdge::Edge* b)
{
    QuadEdge::Edge * e = MakeEdge();
    QuadEdge::Splice(e, a->Lnext());
    QuadEdge::Splice(e->Sym(), b);
    e->SetEndPoints(a->GetDestination(), b->GetOrigin());
    return e;
}
    
void Delaunay::Swap(QuadEdge::Edge* e)
{
    QuadEdge::Edge* a = e->Oprev();
    QuadEdge::Edge* b = e->Sym()->Oprev();
    QuadEdge::Splice(e, a);
    QuadEdge::Splice(e->Sym(), b);
    QuadEdge::Splice(e, a->Lnext());
    QuadEdge::Splice(e->Sym(), b->Lnext());
    e->SetEndPoints(a->GetDestination(), b->GetDestination());
}

Delaunay::Real Delaunay::TriArea(const Point2D & a, const Point2D & b, const Point2D & c)
{
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}
    
int Delaunay::InCircle(const Point2D& a, const Point2D& b, const Point2D& c, const Point2D& d)
{
    return ((a.x*a.x + a.y*a.y) * TriArea(b, c, d) -
            (b.x*b.x + b.y*b.y) * TriArea(a, c, d) +
            (c.x*c.x + c.y*c.y) * TriArea(a, b, d) -
            (d.x*d.x + d.y*d.y) * TriArea(a, b, c) > 0);
}

int Delaunay::ccw(const Point2D& a, const Point2D& b, const Point2D& c)
{
    return (TriArea(a, b, c) > 0);
}

int Delaunay::RightOf(const Point2D & x, const QuadEdge::Edge * e)
{
    return ccw(x, GetDestination(e), GetOrigin(e));
}

int Delaunay::LeftOf(const Point2D & x, const QuadEdge::Edge * e)
{
    return ccw(x, GetOrigin(e), GetDestination(e));
}

int Delaunay::OnEdge(const Point2D & x, const QuadEdge::Edge * e)
{
    Real t1, t2, t3;
    t1 = (x - GetOrigin(e)).norm();
    t2 = (x - GetDestination(e)).norm();
    if(t1 < EPS || t2 < EPS)
        return 1;
    t3 = (GetOrigin(e) - GetDestination(e)).norm();
    if (t1 > t3 || t2 > t3)
        return 0;
    Line line(GetOrigin(e), GetDestination(e));
    return (fabs(line.eval(x)) < EPS); 
}

const Delaunay::Point2D & Delaunay::GetOrigin(const QuadEdge::Edge * e)
{
    if(!e)
    {
        throw Exception("null input in Delaunay::GetOrigin()");
    }
    
    const QuadEdge::Simplex * origin = e->GetOrigin();

    if(! origin)
    {
        throw Exception("null origin in Delaunay::GetOrigin()");
    }

    const Point2D * point2D = dynamic_cast<const Point2D *>(origin);

    if(! point2D)
    {
        throw Exception("null point2D origin in Delaunay::GetOrigin()");
    }

    return *point2D;
}

const Delaunay::Point2D & Delaunay::GetDestination(const QuadEdge::Edge * e)
{
    if(!e)
    {
        throw Exception("null input in Delaunay::GetOrigin()");
    }
    
    const QuadEdge::Simplex * destination = e->GetDestination();

    if(! destination)
    {
        throw Exception("null destination in Delaunay::GetDestination()");
    }

    const Point2D * point2D = dynamic_cast<const Point2D *>(destination);

    if(! point2D)
    {
        throw Exception("null point2D destination in Delaunay::GetDestination()");
    }

    return *point2D;
}

Delaunay::Point2D & Delaunay::GetOrigin(QuadEdge::Edge * e)
{
    if(!e)
    {
        throw Exception("null input in Delaunay::GetOrigin()");
    }
    
    QuadEdge::Simplex * origin = e->GetOrigin();

    if(! origin)
    {
        throw Exception("null origin in Delaunay::GetOrigin()");
    }

    Point2D * point2D = dynamic_cast<Point2D *>(origin);

    if(! point2D)
    {
        throw Exception("null point2D origin in Delaunay::GetOrigin()");
    }

    return *point2D;
}

Delaunay::Point2D & Delaunay::GetDestination(QuadEdge::Edge * e)
{
    if(!e)
    {
        throw Exception("null input in Delaunay::GetOrigin()");
    }
    
    QuadEdge::Simplex * destination = e->GetDestination();

    if(! destination)
    {
        throw Exception("null destination in Delaunay::GetDestination()");
    }

    Point2D * point2D = dynamic_cast<Point2D *>(destination);

    if(! point2D)
    {
        throw Exception("null point2D destination in Delaunay::GetDestination()");
    }

    return *point2D;
}

QuadEdge::Edge * Delaunay::MakeEdge(void)
{
    QuadEdge::Edge * baby = QuadEdge::MakeEdge();
    baby->QEdge()->ID() = _edges.size();
    _edges.push_back(baby);
    
    CheckEdgeConsistency(baby);

    return baby;
}

void Delaunay::DeleteEdge(QuadEdge::Edge * victim)
{
    CheckEdgeConsistency(victim);

    const unsigned int id = victim->QEdge()->ID();
    
    _edges[id] = _edges[_edges.size()-1];
    _edges[id]->QEdge()->ID() = id;

    CheckEdgeConsistency(_edges[id]);

    _edges.pop_back();
    QuadEdge::DeleteEdge(victim);
}

void Delaunay::CheckEdgeConsistency(const QuadEdge::Edge * edge) const
{
    assert(edge);
    
    if(edge)
    {
        const unsigned int id = edge->QEdge()->ID();
        assert((id < _edges.size()) && (_edges[id]->QEdge() == edge->QEdge()));
    }
}

Delaunay::Vector2D::Vector2D(void) : x(0), y(0)
{
    // nothing to do
}

Delaunay::Vector2D::Vector2D(const Real a, const Real b) : x(a), y(b)
{
    // nothing to do
}

Delaunay::Real Delaunay::Vector2D::norm(void) const
{
    return sqrt(x*x + y*y);
}

void Delaunay::Vector2D::normalize(void)
{
    const Real length = norm();
    if(length > 0)
    {
        x /= length;
        y /= length;
    }
}

Delaunay::Vector2D Delaunay::Vector2D::operator+(const Vector2D & v) const
{
    return Vector2D(x + v.x, y + v.y);
}

Delaunay::Vector2D Delaunay::Vector2D::operator-(const Vector2D & v) const
{
    return Vector2D(x - v.x, y - v.y);
}

Delaunay::Vector2D Delaunay::Vector2D::operator*(Real c, const Vector2D & v)
{
    return Vector2D(c * v.x, c * v.y);
}

Delaunay::Real Delaunay::Vector2D::dot(const Vector2D & a, const Vector2D & b)
{
    return (a.x*b.x + a.y*b.y);
}
     
Delaunay::Point2D::Point2D(void) : x(0), y(0), category(FREE)
{
    // nothing to do
}

Delaunay::Point2D::Point2D(const Real x_in, const Real y_in) : x(x_in), y(y_in), category(FREE)
{
    // nothing to do
}

Delaunay::Point2D::Point2D(const Real x_in, const Real y_in, const Category category_in) : x(x_in), y(y_in), category(category_in)
{
    // nothing to do
}

Delaunay::Point2D::~Point2D(void)
{
    // nothing to do
}

Delaunay::Point2D * Delaunay::Point2D::Clone(void) const
{
    return new Point2D(*this);
}

Delaunay::Point2D Delaunay::Point2D::operator+(const Vector2D & v) const
{
    return Point2D(x + v.x, y + v.y);
}

Delaunay::Vector2D Delaunay::Point2D::operator-(const Point2D & p) const
{
    return Vector2D(x - p.x, y - p.y);
}

int Delaunay::Point2D::operator==(const Point2D & p) const
{
    return ((*this - p).norm() < EPS);
}

Delaunay::Line::Line(void) : _a(0), _b(0), _c(0)
{
    // nothing to do
}

Delaunay::Line::Line(const Point2D & p, const Point2D & q)
{
    Vector2D t = q - p;
    Real len = t.norm();
    _a =   t.y / len;
    _b = - t.x / len;
    _c = -(_a*p.x + _b*p.y);
}

Delaunay::Real Delaunay::Line::eval(const Point2D & p) const
{
    return (_a * p.x + _b* p.y + _c);
}

int Delaunay::Line::classify(const Point2D & p) const
{
    Real d = eval(p);
    return (d < -EPS) ? -1 : (d > EPS ? 1 : 0);
}

const Delaunay::Real Delaunay::EPS = 1e-6;
