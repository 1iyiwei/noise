/*
  LloydRelaxation.cpp

  Li-Yi Wei
  11/01/2007

*/

#include "LloydRelaxation.hpp"
#include "SequentialCounter.hpp"
#include "Exception.hpp"

#include <iostream>
#include <algorithm>
using namespace std;

#include <math.h>
#include <assert.h>

template <class T>
T mod(const T a, const T b)
{
    return (a-floor(a/b)*b);
}

string LloydRelaxation::RunOnce(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const vector<Point2D *> & input)
{
    // error checking
    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(!input[i])
        {
            return "null input point";
        }

        input[i]->x = Perturb(input[i]->x, x_min, x_max);
        input[i]->y = Perturb(input[i]->y, y_min, y_max);
    }

    // compute delaunay triangulation of cloned output points
    vector<Point2D *> output(input.size());
    for(unsigned int i = 0; i < output.size(); i++)
    {
        output[i] = input[i]->Clone();
        assert(output[i]);
    }

    // for toroidal boundary condition only
    vector<Point2D *> clone;
    if(boundary_condition == TOROIDAL)
    {
        for(unsigned int i = 0; i < input.size(); i++)
        {
            for(int row = -1; row <= 1; row++)
                for(int col = -1; col <= 1; col++)
                {
                    if((row != 0) || (col != 0)) // skip output itself
                    {
                        Point2D * baby = input[i]->Clone();

                        baby->x += row*(x_max-x_min);
                        baby->y += col*(y_max-y_min);

                        clone.push_back(baby);
                    }
                }
        }
    }

    Delaunay * mesh = 0;

    if(boundary_condition == TOROIDAL)
    {
        mesh = new Delaunay(2*x_min - x_max, 2*x_max - x_min, 2*y_min - y_max, 2*y_max - y_min);
    }
    else
    {
        mesh = new Delaunay(x_min, x_max, y_min, y_max);
    }

    for(unsigned int i = 0; i < output.size(); i++)
    {
        if(! mesh->InsertSite(*output[i]))
        {
            return "cannot insert site";
        }
    }
    for(unsigned int i = 0; i < clone.size(); i++)
    {
        if(! mesh->InsertSite(*clone[i]))
        {
            return "cannot insert site";
        }
    }

    // compute voronoi centroid into input points
    for(unsigned int i = 0; i < output.size(); i++)
    {
        if(output[i]->NumEdgeRings() != 1)
        {
            cerr << "output[" << i << "]->NumEdgeRings() " << output[i]->NumEdgeRings() << endl;
            return "NumEdgeRings() != 1";
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
            if(boundary_condition == TOROIDAL)
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
            cerr << "out of range (" << input[i]->x << ", " << input[i]->y << "), neighbors " << neighbors.size() << " :";
            for(unsigned int j = 0; j < neighbors.size(); j++)
            {
                cerr << " (" << neighbors[j]->x << ", " << neighbors[j]->y << ")";
            }
            cerr << endl;
            
            throw Exception("point out of range");
        }
#endif
    }
    
    // clean up
    delete mesh;

    for(unsigned int i = 0; i < output.size(); i++)
    {
        delete output[i];
    }
    output.clear();

    for(unsigned int i = 0; i < clone.size(); i++)
    {
        delete clone[i];
    }
    clone.clear();
    
    // done
    return "";
}

string LloydRelaxation::Voronoi(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const vector<const Point2D *> & input, vector<VoronoiRegion> & regions)
{
    // error checking
    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(!input[i])
        {
            return "null input point";
        }
    }

    // compute delaunay triangulation of cloned output points
    vector<Point2D *> output(input.size());
    for(unsigned int i = 0; i < output.size(); i++)
    {
        output[i] = input[i]->Clone();
        assert(output[i]);
    }

    // for toroidal boundary condition only
    vector<Point2D *> clone;
    if(boundary_condition == TOROIDAL)
    {
        for(unsigned int i = 0; i < input.size(); i++)
        {
            for(int row = -1; row <= 1; row++)
                for(int col = -1; col <= 1; col++)
                {
                    if((row != 0) || (col != 0)) // skip output itself
                    {
                        Point2D * baby = input[i]->Clone();

                        baby->x += row*(x_max-x_min);
                        baby->y += col*(y_max-y_min);

                        clone.push_back(baby);
                    }
                }
        }
    }

    Delaunay * mesh = 0;

#if 1
    const Real local_x_min = (boundary_condition == TOROIDAL ? 2*x_min - x_max : x_min);
    const Real local_x_max = (boundary_condition == TOROIDAL ? 2*x_max - x_min : x_max);
    const Real local_y_min = (boundary_condition == TOROIDAL ? 2*y_min - y_max : y_min);
    const Real local_y_max = (boundary_condition == TOROIDAL ? 2*y_max - y_min : y_max);

    mesh = new Delaunay(local_x_min, local_x_max, local_y_min, local_y_max);
#else
    if(boundary_condition == TOROIDAL)
    {
        mesh = new Delaunay(2*x_min - x_max, 2*x_max - x_min, 2*y_min - y_max, 2*y_max - y_min);
    }
    else
    {
        mesh = new Delaunay(x_min, x_max, y_min, y_max);
    }
#endif

    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(! mesh->InsertSite(*output[i]))
        {
            return "cannot insert site";
        }
    }
    for(unsigned int i = 0; i < clone.size(); i++)
    {
        if(! mesh->InsertSite(*clone[i]))
        {
            return "cannot insert site";
        }
    }

#if 1
    output.insert(output.end(), clone.begin(), clone.end());
    clone.clear();
#endif

    regions.clear();
    VoronoiRegion region;

    // compute voronoi regions
    for(unsigned int i = 0; i < output.size(); i++)
    {
        if(output[i]->NumEdgeRings() != 1)
        {
            cerr << "output[" << i << "]->NumEdgeRings() " << output[i]->NumEdgeRings() << endl;
            return "NumEdgeRings() != 1";
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
                sorties[j].value = Angle(Point2D(neighbors_input[j]->x-center.x, neighbors_input[j]->y-center.y)); 
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

        if(!any_neighbor_local_region_corner && !any_voronoi_vertex_outside_region)
        {
            regions.push_back(region);
        }
    }

    // clean up
    delete mesh;

    for(unsigned int i = 0; i < output.size(); i++)
    {
        delete output[i];
    }
    output.clear();

    for(unsigned int i = 0; i < clone.size(); i++)
    {
        delete clone[i];
    }
    clone.clear();
    
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
        Real min_distance = Distance(x_min, x_max, y_min, y_max, boundary_condition, *points[i], *snappers[min_index]);

        for(unsigned int j = 1; j < snappers.size(); j++)
        {
            const Real distance = Distance(x_min, x_max, y_min, y_max, boundary_condition, *points[i], *snappers[j]);

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

LloydRelaxation::Real LloydRelaxation::Distance(const Point2D & p1, const Point2D & p2)
{
    const Real x_diff = p1.x - p2.x;
    const Real y_diff = p1.y - p2.y;
    return x_diff*x_diff + y_diff*y_diff;
}

LloydRelaxation::Real LloydRelaxation::Distance(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const BoundaryCondition boundary_condition, const Point2D & p1, const Point2D & p2)
{
    if(boundary_condition == NONE)
    {
        return Distance(p1, p2);
    }
    else if(boundary_condition == TOROIDAL)
    {
        SequentialCounter counter(2, -1, 1);
        Real min_distance = Distance(p1, p2);

        vector<int> index(2);

        Point2D proxy;

        counter.Reset();
        do
        {
            counter.Get(index);

            proxy.x = p1.x + index[0]*(x_max - x_min);
            proxy.y = p1.y + index[1]*(y_max - y_min);

            Real distance = Distance(proxy, p2);

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

#include <iostream>
using namespace std;

int LloydRelaxation::Centroid(const Point2D & center, const vector<const Point2D *> & neighbors_input, Point2D & centroid)
{
    // sort input neighbors in ccw order
    vector<Sortie> sorties(neighbors_input.size());
    for(unsigned int i = 0; i < sorties.size(); i++)
    {
        sorties[i].index = i;
        sorties[i].value = Angle(Point2D(neighbors_input[i]->x-center.x, neighbors_input[i]->y-center.y)); 
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
            angles[i] = Angle(Point2D(neighbors[i]->x-center.x, neighbors[i]->y-center.y)); 
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
            cerr << "angles " << angles.size() << ":";
            for(unsigned int i = 0; i < angles.size(); i++)
            {
                cerr << " " << angles[i];
            }
            cerr << endl;

            throw Exception("neighbors not ordered in LloydRelaxation::Centroid()");
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
        cerr << "centroid (" << centroid.x-center.x << ", " << centroid.y-center.y << "), polygon " << polygon.size() << " :";

        for(unsigned int i = 0; i < polygon.size(); i++)
        {
            cerr << " (" << polygon[i]->x-center.x << ", " << polygon[i]->y-center.y << ")";
        }
        cerr << endl;
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
        throw Exception("null det in LloydRelaxation::VoronoiVertex()");
        return 0;
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

LloydRelaxation::Real LloydRelaxation::Angle(const Point2D & point)
{
    Delaunay::Vector2D normalized(point.x, point.y);
    normalized.normalize();
    
    return (normalized.y >= 0 ? acos(normalized.x) : 2*acos(-1.0)-acos(normalized.x));
}

LloydRelaxation::Real LloydRelaxation::Perturb(const Real value, const Real minimum, const Real maximum)
{
    const Real very_small = (maximum - minimum) * 10e-15;
    Real result = value;

    if(result == floor(result))
    {
        if((result-minimum) < (maximum-result))
        {
            result += very_small;
        }
        else
        { 
            result -= very_small;
        }

        cerr << "perturb " << value << " to " << result << endl;
    }

    return result;
}
