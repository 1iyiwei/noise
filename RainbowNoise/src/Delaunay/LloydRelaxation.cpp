/*
  LloydRelaxation.cpp

  Li-Yi Wei
  11/01/2007

*/

#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

#include <math.h>
#include <assert.h>

#include "LloydRelaxation.hpp"
#include "SequentialCounter.hpp"
#include "Math2D.hpp"
#include "Random.hpp"
#include "Exception.hpp"

template <class T>
T mod(const T a, const T b)
{
    return (a-floor(a/b)*b);
}

string LloydRelaxation::RunOnce(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const vector<Point2D *> & input)
{
    return RunOnce(x_min, x_max, y_min, y_max, _VERY_SMALL, boundary_condition, input);
}

string LloydRelaxation::RunOnce(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D *> & input)
{
    // error checking
    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(!input[i])
        {
            return "null input point";
        }

        input[i]->x = Perturb(input[i]->x, x_min, x_max, very_small);
        input[i]->y = Perturb(input[i]->y, y_min, y_max, very_small);
    }

    DelaunayMesh dmesh(DelaunayMesh::Box(x_min, x_max, y_min, y_max), very_small, boundary_condition, input);

    Delaunay * mesh = dmesh.Mesh();
    vector<Point2D *> output = dmesh.Vertices();

    // compute voronoi centroid into input points
    for(unsigned int i = 0; i < output.size(); i++)
    {
        if(output[i]->category == Point2D::ORPHAN)
        {
            // assume output.size() == input.size();
            input[i]->category = output[i]->category;
            continue;
        }

        if(output[i]->NumEdgeRings() != 1)
        {
            stringstream message;
            message << "RunOnce(): output[" << i << "]->NumEdgeRings() == " << output[i]->NumEdgeRings() << " != 1 (" << output[i]->id << "," << output[i]->x << ", " << output[i]->y << ")";
            return message.str();
        }

        vector<const Point2D *> neighbors;
        
        const QuadEdge::Edge * first_edge = output[i]->FirstEdge(0);
        
        assert(first_edge);

        const QuadEdge::Edge * edge = first_edge;

        int no_move = (output[i]->category != Delaunay::Point2D::FREE);
        
        do
        {
            edge = edge->Onext();
            const Delaunay::Point2D & destination = Delaunay::GetDestination(edge);

#if 1
            if(destination.category != Delaunay::Point2D::FREE)
            {
                no_move = 1;
            }
#endif
            
            //if(destination.category == Delaunay::Point2D::FREE)
            // need to include anchor points as well so as to
            // ensure that every point is properly surrounded
            {
                neighbors.push_back(&destination);
            }
        }
        while(edge != first_edge);

        if(no_move) continue;
        
        Point2D centroid(input[i]->x, input[i]->y);
        if((neighbors.size() > 2) && !Centroid(*output[i], neighbors, centroid))
        {
            return "cannot compute centroid";
        }

        // bring it back into range
        if((centroid.x < x_min) || (centroid.x > x_max) || (centroid.y < y_min) || (centroid.y > y_max))
        {
            if(boundary_condition == DelaunayMesh::TOROIDAL)
            {
                input[i]->x = mod(centroid.x, x_max-x_min) + x_min; 
                input[i]->y = mod(centroid.y, y_max-y_min) + y_min;
            }
            else
            {
                // out of range, no update
            }
        }
        else
        {
            input[i]->x = centroid.x;   
            input[i]->y = centroid.y;
        }
        
#ifndef NDEBUG
        // check to see if out of range
        if((input[i]->x < x_min) || (input[i]->x > x_max) || (input[i]->y < y_min) || (input[i]->y > y_max))
        {
            stringstream message;

            message << "out of range (" << input[i]->x << ", " << input[i]->y << "), neighbors " << neighbors.size() << " :";
            for(unsigned int j = 0; j < neighbors.size(); j++)
            {
                message << " (" << neighbors[j]->x << ", " << neighbors[j]->y << ")";
            }
            message << endl;
            
            throw Exception(message.str() + "point out of range");
        }
#endif
    }
    
    // done
    return "";
}

string LloydRelaxation::Voronoi(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const vector<const Point2D *> & input, vector<VoronoiRegion> & regions)
{
    return Voronoi(x_min, x_max, y_min, y_max, _VERY_SMALL, boundary_condition, true, input, regions);
}

string LloydRelaxation::Voronoi(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<const Point2D *> & input, vector<VoronoiRegion> & regions)
{
    return Voronoi(x_min, x_max, y_min, y_max, very_small, boundary_condition, true, input, regions);
}

string LloydRelaxation::Voronoi(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const bool with_clone, const vector<const Point2D *> & input, vector<VoronoiRegion> & regions)
{
    // error checking
    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(!input[i])
        {
            return "null input point";
        }
    }

    DelaunayMesh dmesh(DelaunayMesh::Box(x_min, x_max, y_min, y_max), very_small, boundary_condition, input);

    Delaunay * mesh = dmesh.Mesh();
    vector<Point2D *> output = dmesh.Vertices();
    vector<Point2D *> clone = dmesh.Clones();
    DelaunayMesh::Box box = dmesh.OuterBox();

    const Real local_x_min = box.x_min;
    const Real local_x_max = box.x_max;
    const Real local_y_min = box.y_min;
    const Real local_y_max = box.y_max;

    if(0)
    {
        // clip
        if( Clip(local_x_min, local_x_max, local_y_min, local_y_max, very_small, output) < 0)
        {
            return "error in clipping";
        }

        if( Clip(local_x_min, local_x_max, local_y_min, local_y_max, very_small, clone) < 0)
        {
            return "error in clipping";
        }
    }

    if(with_clone)
    {
        output.insert(output.end(), clone.begin(), clone.end());
        clone.clear();
    }

    regions.clear();
    VoronoiRegion region;

    // compute voronoi regions
    for(unsigned int i = 0; i < output.size(); i++)
    {
        if(output[i]->category == Point2D::ORPHAN) continue;

        if(output[i]->NumEdgeRings() != 1)
        {
            stringstream message;
            message << "Voronoi(): output[" << i << "]->NumEdgeRings() == " << output[i]->NumEdgeRings() << " != 1 (" << output[i]->id << "," << output[i]->x << ", " << output[i]->y << ")";
            return message.str();
        }

        vector<const Point2D *> neighbors;
        
        const QuadEdge::Edge * first_edge = output[i]->FirstEdge(0);
        
        assert(first_edge);

        const QuadEdge::Edge * edge = first_edge;

        do
        {
            edge = edge->Onext();
            const Delaunay::Point2D & destination = Delaunay::GetDestination(edge);
 
            //if(destination.category == Delaunay::Point2D::FREE)
            // need to include anchor points as well so as to
            // ensure that every point is properly surrounded
            {
                neighbors.push_back(&destination);
            }
        }
        while(edge != first_edge);

        // sort input neighbors in ccw order
        {
            const Point2D & center = *output[i];
         
            const vector<const Point2D *> neighbors_input = neighbors;

            vector<Sortie> sorties(neighbors_input.size());
            for(unsigned int j = 0; j < sorties.size(); j++)
            {
                sorties[j].index = j;
                sorties[j].value = Math2D::Angle(Point2D(neighbors_input[j]->x-center.x, neighbors_input[j]->y-center.y)); 
            }
            sort(sorties.begin(), sorties.end());
        
            for(unsigned int j = 0; j < neighbors.size(); j++)
            {
                neighbors[j] = neighbors_input[sorties[j].index];
            }
        }

        region.center = *output[i];
        region.ring = vector<Point2D>(neighbors.size());

        for(unsigned int j = 0; j < region.ring.size(); j++)
        {
            if(! VoronoiVertex(region.center, *neighbors[j], *neighbors[(j+1)%neighbors.size()], region.ring[j]))
            {
                return "cannot compute Voronoi vertex";
            }
        }
        
        bool any_neighbor_local_region_corner = 0;
#if 0
        for(unsigned int j = 0; j < neighbors.size(); j++)
        {
            const Point2D & neighbor = *neighbors[j];
            if(((neighbor.x == local_x_min) || (neighbor.x == local_x_max)) && ((neighbor.y == local_y_min) || (neighbor.y == local_y_max)))
            {
                any_neighbor_local_region_corner = 1;
            }
        }
#endif

        bool any_voronoi_vertex_outside_region = 0;

        for(unsigned int j = 0; j < region.ring.size(); j++)
        {
            const Point2D & neighbor = region.ring[j];

            if((neighbor.x < local_x_min) || (neighbor.x > local_x_max) || (neighbor.y < local_y_min) || (neighbor.y > local_y_max))
            {
                any_voronoi_vertex_outside_region = 1;
            }
        }

        bool center_outside_domain = !((region.center.x >= x_min) && (region.center.x <= x_max) && (region.center.y >= y_min) && (region.center.y <= y_max));

        if(!with_clone || (!any_neighbor_local_region_corner && (!any_voronoi_vertex_outside_region/* || (boundary_condition == NONE)*/) && !center_outside_domain))
        {
            regions.push_back(region);
        }
    }

    // done
    return "";
}

string LloydRelaxation::Centroid(const VoronoiRegion & region, Point2D & centroid)
{
    vector<Point2D *> neighbors(region.ring.size());

    for(unsigned int i = 0; i < neighbors.size(); i++)
    {
        neighbors[i] = const_cast<Point2D *>(&region.ring[i]);
    }

    if(!Centroid(neighbors, centroid))
    {
        return "error";
    }
    else
    {
        return "";
    }
}

LloydRelaxation::Real LloydRelaxation::Area(const VoronoiRegion & region)
{
    Real area = 0;

    const vector<Point2D> & polygon = region.ring;

    for(unsigned int i = 0; i < polygon.size(); i++)
    {
        const int j = (i+1)%polygon.size();

        const Delaunay::Point2D & polygon_i = polygon[i];
        const Delaunay::Point2D & polygon_j = polygon[j];
        
        const Real ai = polygon_i.x*polygon_j.y - polygon_j.x*polygon_i.y;
        area += ai;
    }

    area /= 2;

    return area;
}

string LloydRelaxation::LargestEmptyCircle(const Real x_min, const Real x_max, const Real y_min, const Real y_max,
    const vector<VoronoiRegion> &regions, Real &x, Real &y, Real &radius)
{
    radius = -1.0;

    Delaunay::AABB aabb(x_min, x_max, y_min, y_max);

    // TODO: Remove the repeated computation due to vertices shared by multiple regions
    for (unsigned int i = 0; i < regions.size(); ++i)
    {
        if (!aabb.Inside(regions[i].center))
        {
            continue;
        }
        for (unsigned int j = 0; j < regions[i].ring.size(); ++j)
        {
            if (!aabb.Inside(regions[i].ring[j]))
            {
                continue;
            }

            Real r = (regions[i].ring[j] - regions[i].center).norm();
            if (r > radius)
            {
                radius = r;
                x = regions[i].ring[j].x;
                y = regions[i].ring[j].y;
            }
        }
    }

    // done
    return "";
}

string LloydRelaxation::Snap(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const vector<Point2D *> & points, const vector<const Point2D *> & snappers)
{
    // error checking
    for(unsigned int i = 0; i < points.size(); i++)
    {
        if(! points[i]) return "null input";
    }
    for(unsigned int i = 0; i < snappers.size(); i++)
    {
        if(! snappers[i]) return "null snapper";
    }

    if(points.size() <= 0) return "empty points";
    if(snappers.size() <= 0) return "empty snappers";

    for(unsigned int i = 0; i < points.size(); i++)
    {
        int min_index = 0;
        Real min_distance = Distance2(x_min, x_max, y_min, y_max, boundary_condition, *points[i], *snappers[min_index]);

        for(unsigned int j = 1; j < snappers.size(); j++)
        {
            const Real distance = Distance2(x_min, x_max, y_min, y_max, boundary_condition, *points[i], *snappers[j]);

            if(distance < min_distance)
            {
                min_distance = distance;
                min_index = j;
            }
        }

        points[i]->x = snappers[min_index]->x;
        points[i]->y = snappers[min_index]->y;
    }

    // done
    return "";
}

LloydRelaxation::Real LloydRelaxation::Distance2(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const Point2D & p1, const Point2D & p2)
{
    if(boundary_condition == DelaunayMesh::NONE)
    {
        return Math2D::Distance2(p1, p2);
    }
    else if(boundary_condition == DelaunayMesh::TOROIDAL)
    {
        SequentialCounter counter(2, -1, 1);
        Real min_distance = Math2D::Distance2(p1, p2);

        vector<int> index(2);

        Point2D proxy;

        counter.Reset();
        do
        {
            counter.Get(index);

            proxy.x = p1.x + index[0]*(x_max - x_min);
            proxy.y = p1.y + index[1]*(y_max - y_min);

            Real distance = Math2D::Distance2(proxy, p2);

            if(distance < min_distance)
            {
                min_distance = distance;
            }
        }
        while(counter.Next());

        return min_distance;
    }
    else
    {
        throw Exception("unknown boundary condition");
        return -1;
    }
}

int LloydRelaxation::Centroid(const Point2D & center, const vector<const Point2D *> & neighbors_input, Point2D & centroid)
{
    // sort input neighbors in ccw order
    vector<Sortie> sorties(neighbors_input.size());
    for(unsigned int i = 0; i < sorties.size(); i++)
    {
        sorties[i].index = i;
        sorties[i].value = Math2D::Angle(Point2D(neighbors_input[i]->x-center.x, neighbors_input[i]->y-center.y)); 
    }
    sort(sorties.begin(), sorties.end());
    
    vector<const Point2D *> neighbors(neighbors_input.size());
    for(unsigned int i = 0; i < neighbors.size(); i++)
    {
        neighbors[i] = neighbors_input[sorties[i].index];
    }

#ifndef NDEBUG
    // check to make sure that neighbors are sorted in ccw order
    {
        vector<float> angles(neighbors.size());

        assert(angles.size() > 0);
        
        for(unsigned int i = 0; i < angles.size(); i++)
        {
            angles[i] = Math2D::Angle(Point2D(neighbors[i]->x-center.x, neighbors[i]->y-center.y)); 
        }

        Real min_angle = 2*2*acos(-1.0);
        int min_index = -1;
        
        for(unsigned int i = 0; i < angles.size(); i++)
        {
            if(angles[i] <= min_angle)
            {
                min_angle = angles[i];
                min_index = i;
            }
        }
        
        assert(min_index >= 0);

        int not_sorted = 0;
        for(int i = min_index; (i+1)%angles.size() != min_index; i = (i+1)%angles.size())
        {
            if(angles[i] > angles[(i+1)%angles.size()])
            {
                not_sorted = 1;
            }
        }

        if(not_sorted)
        {
            stringstream message;
            message << "angles " << angles.size() << ":";
            for(unsigned int i = 0; i < angles.size(); i++)
            {
                message << " " << angles[i];
            }
            message << endl;

            throw Exception(message.str() + "neighbors not ordered in LloydRelaxation::Centroid()");
        }
    }
#endif
    
    // real job
    vector<Point2D *> polygon(neighbors.size());
    for(unsigned int i = 0; i < polygon.size(); i++)
    {
        polygon[i] = neighbors[i]->Clone();
#if 1
        VoronoiVertex(center, *neighbors[i], *neighbors[(i+1)%neighbors.size()], *polygon[i]);
#else
        polygon[i]->x = (neighbors[i]->x + center.x)/2;
        polygon[i]->y = (neighbors[i]->y + center.y)/2;
#endif
    }

    const int status = Centroid(polygon, centroid);

#if 0
    // debug
    if(1)
    {
        stringstream message;

        message << "centroid (" << centroid.x-center.x << ", " << centroid.y-center.y << "), polygon " << polygon.size() << " :";

        for(unsigned int i = 0; i < polygon.size(); i++)
        {
            message << " (" << polygon[i]->x-center.x << ", " << polygon[i]->y-center.y << ")";
        }
        message << endl;

        cerr << message;
    }
#endif
    
    for(unsigned int i = 0; i < polygon.size(); i++)
    {
        delete polygon[i];
    }
    
    return status;
}

int LloydRelaxation::VoronoiVertex(const Point2D & c, const Point2D & n0, const Point2D & n1, Point2D & result)
{
    const Delaunay::Vector2D h0((c.x + n0.x)/2, (c.y + n0.y)/2);
    const Delaunay::Vector2D h1((c.x + n1.x)/2, (c.y + n1.y)/2);
    const Delaunay::Vector2D d0(n0.x-c.x, n0.y-c.y);
    const Delaunay::Vector2D d1(n1.x-c.x, n1.y-c.y);

    const Real a0 = d0.x; const Real b0 = d0.y;
    const Real c0 = h0.x*d0.x + h0.y*d0.y;
    const Real a1 = d1.x; const Real b1 = d1.y;
    const Real c1 = h1.x*d1.x + h1.y*d1.y;

    const Real det = (a0*b1 - a1*b0);

    if(det != 0)
    {
        result.x = (b1*c0 - b0*c1)/det;
        result.y = (a0*c1 - a1*c0)/det;
    }
    else
    {
#if 1
        // for 3 co-linear points, simply pick the closer one as result
        // not entirely correct, as we need to check consecutive co-linears
        if(Math2D::Distance2(c, h0) > Math2D::Distance2(c, h1))
        {
            result = h1;
        }
        else
        {
            result = h0;
        }
#else
        throw Exception("null det in LloydRelaxation::VoronoiVertex()");
        return 0;
#endif
    }
    
    return 1;
}

int LloydRelaxation::Centroid(const vector<Point2D *> & polygon, Point2D & centroid)
{
    // adopted from "Centroid of a Polygon" in Graphics Gems IV

    if(polygon.size() < 3)
    {
        return 0;
    }

    Real area = 0;
    centroid.x = centroid.y = 0;

    for(unsigned int i = 0; i < polygon.size(); i++)
    {
        const int j = (i+1)%polygon.size();

        const Delaunay::Point2D & polygon_i = *polygon[i];
        const Delaunay::Point2D & polygon_j = *polygon[j];
        
        const Real ai = polygon_i.x*polygon_j.y - polygon_j.x*polygon_i.y;
        area += ai;
        centroid.x += (polygon_i.x + polygon_j.x)*ai;
        centroid.y += (polygon_i.y + polygon_j.y)*ai;
    }

    area /= 2;

    if(area == 0)
    {
        return 0;
    }
    else
    {
        centroid.x /= 6*area; centroid.y /= 6*area;

        return 1;
    }
}

int LloydRelaxation::Clip(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, vector<Point2D *> & input)
{
    vector<Point2D *> output;

    const Real x_delta = fabs((x_max - x_min)*very_small);
    const Real y_delta = fabs((y_max - y_min)*very_small);

    for(unsigned int i = 0; i < input.size(); i++)
    {
        const Point2D & point = *input[i];

        if((point.x <= (x_min + x_delta)) || 
           (point.x >= (x_max - x_delta)) || 
           (point.y <= (y_min + y_delta)) || 
           (point.y >= (y_max - y_delta)) )
        {
            delete input[i];
            input[i] = 0;
        }
    }

    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(input[i])
        {
            output.push_back(input[i]);
        }
    }

    const int kill_count = input.size() - output.size();

    input = output;

    return kill_count;
}

LloydRelaxation::Real LloydRelaxation::Perturb(const Real value, const Real minimum, const Real maximum, const Real very_small_ratio)
{
    const Real very_small = (maximum - minimum) * very_small_ratio;
    Real result = value;

    //if(result == floor(result))
    {
        if((result-minimum) < (maximum-result))
        {
            result += very_small*(0.5*Random::UniformRandom()+0.5);
        }
        else
        { 
            result -= very_small*(0.5*Random::UniformRandom()+0.5);
        }

        // cerr << "perturb " << value << " to " << result << endl;
    }

    return result;
}
 
const LloydRelaxation::Real LloydRelaxation::_VERY_SMALL = 10e-15;
