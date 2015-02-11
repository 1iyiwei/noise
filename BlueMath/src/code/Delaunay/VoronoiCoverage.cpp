/*
  VoronoiCoverage.cpp

  Li-Yi Wei
  11/07/2013

*/

#include <cmath>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;

#include "VoronoiCoverage.hpp"
#include "Math.hpp"
#include "Math2D.hpp"
#include "Exception.hpp"

VoronoiCoverage::~VoronoiCoverage(void)
{
    // nothing else to do
}

VoronoiCoverage::Real VoronoiCoverage::Integral(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D> & points, const Real power) const
{
    vector<const Point2D *> points_ptr(points.size());

    for(unsigned int i = 0; i < points_ptr.size(); i++)
    {
        points_ptr[i] = & points[i];
    }

    Mesh mesh;

    const string message = LloydRelaxation::Voronoi(x_min, x_max, y_min, y_max, very_small, boundary_condition, points_ptr, mesh.polygons);

    if(message != "")
    {
        throw Exception("Error in VoronoiCoverage::Integral(): " + message);
    }

    const Real answer = MeshIntegral(mesh, power);

    return answer;
}

vector<VoronoiCoverage::Vector2D> VoronoiCoverage::Gradient(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const Real very_small, const BoundaryCondition boundary_condition, const vector<Point2D> & points, const Real power) const
{
    vector<const Point2D *> points_ptr(points.size());

    for(unsigned int i = 0; i < points_ptr.size(); i++)
    {
        points_ptr[i] = & points[i];
    }

    Mesh mesh;

    const string message = LloydRelaxation::Voronoi(x_min, x_max, y_min, y_max, very_small, boundary_condition, points_ptr, mesh.polygons);

    if(message != "")
    {
        throw Exception("Error in VoronoiCoverage::Integral(): " + message);
    }

    return MeshGradient(mesh, power);
}

VoronoiCoverage::Real VoronoiCoverage::Mintegral(const Real x_min, const Real x_max, const Real y_min, const Real y_max, const int num_points, const Real power) const
{
    const Real area_per_triangle = fabs((x_max - x_min)*(y_max - y_min)/(max(num_points, 1)*6));

    const Real equi_tri_side_length = sqrt(4*area_per_triangle/sqrt(3.0));

    Real answer = 0;

    if(Math::IsINF(power))
    {
        answer = equi_tri_side_length;
    }
    else
    {
        Point2D start, end;
        
        start.x = equi_tri_side_length;
        start.y = 0;

        const Real angle_60 = Math::PI/3;
        end.x = equi_tri_side_length*cos(angle_60);
        end.y = equi_tri_side_length*sin(angle_60);
        
        answer = num_points*6*TriIntegral(start, end, power);
        
        if(power > 0)
        {
            answer = pow(answer, 1.0/power);
        }
    }

    return answer;
}

VoronoiCoverage::Real VoronoiCoverage::MeshIntegral(const Mesh & mesh, const Real power) const
{
    Real answer = 0;

    if(power < 0)
    {
        throw Exception("VoronoiCoverage::MeshIntegral(): negative power!");

        answer = -1; // error
    }
    else if(Math::IsINF(power))
    {
        answer = 0;

        for(unsigned int i = 0; i < mesh.polygons.size(); i++)
        {
            const Real polygon_ans = Max(mesh.polygons[i]);

            if(polygon_ans > answer)
            {
                answer = polygon_ans;
            }
        }
    }
    else
    {
        answer = 0;
        for(unsigned int i = 0; i < mesh.polygons.size(); i++)
        {
            const Real polygon_ans = PolyIntegral(mesh.polygons[i], power);
            answer += polygon_ans;
        }

        if(power > 0)
        {
            answer = pow(answer, 1.0/power);
        }
    }

    return answer;
}

std::vector<VoronoiCoverage::Vector2D> VoronoiCoverage::MeshGradient(const Mesh & mesh, const Real power) const
{
    std::vector<VoronoiCoverage::Vector2D> answer;
    answer.resize(mesh.polygons.size());

    if(power < 0)
    {
        throw Exception("VoronoiCoverage::MeshGradient(): negative power!");
        return answer;
    }
    else if(Math::IsINF(power))
    {
        throw Exception("VoronoiCoverage::MeshGradient(): infinite power!");
        return answer;
    }
    else
    {
        for(unsigned int i = 0; i < mesh.polygons.size(); i++)
        {
            const Vector2D polygon_ans = PolyIntegralGradient(mesh.polygons[i], power);
            answer[i] = polygon_ans;
        }
        // The 1/power exponent is dropped for now
    }
    return answer;
}

VoronoiCoverage::Real VoronoiCoverage::Max(const Polygon & polygon)
{
    Real answer = 0;
    
    for(unsigned int i = 0; i < polygon.ring.size(); i++)
    {
        Real vertex_ans = Distance(polygon.ring[i], polygon.center);

        if(vertex_ans > answer)
        {
            answer = vertex_ans;
        }
    }

    return answer;
}

VoronoiCoverage::Real VoronoiCoverage::PolyIntegral(const Polygon & polygon, const Real power) const
{
    Real answer = 0;

    for(unsigned int i = 0; i < polygon.ring.size(); i++)
    {
        const Point2D start = polygon.ring[i] - polygon.center;
        const Point2D end = polygon.ring[(i+1)%polygon.ring.size()] - polygon.center;

        Real triangle_answer = TriIntegral(start, end, power);

        answer += triangle_answer;
    }

#if 0
    cerr << "polygon.center: " << polygon.center.x << ", " << polygon.center.y << endl;
    for(unsigned int i = 0; i < polygon.ring.size(); i++)
    {
        cerr << "polygon.ring[" << i << "]: " << polygon.ring[i].x << ", " << polygon.ring[i].y << endl;
    }

    cerr << "answer: " << answer << endl;
#endif

    return answer;
}

VoronoiCoverage::Real VoronoiCoverage::TriIntegral(const Point2D & start, const Point2D & end, const Real power) const
{
    throw Exception("VoronoiCoverage::TriIntegral(): shouldn't be called");

    return 0;
}

VoronoiCoverage::Vector2D VoronoiCoverage::PolyIntegralGradient(const Polygon & polygon, const Real power) const
{
    Vector2D gradient;
    for(size_t i = 0; i < polygon.ring.size(); i++)
    {
        const Point2D start = polygon.ring[i] - polygon.center;
        const Point2D end = polygon.ring[(i+1)%polygon.ring.size()] - polygon.center;

        Vector2D triangle_gradient = TriIntegralGradient(start, end, power);
        gradient = gradient + triangle_gradient;
    }
    return gradient;
}

// compute integral of TriIntegral
VoronoiCoverage::Vector2D VoronoiCoverage::TriIntegralGradient(const Point2D & start, const Point2D & end, const Real power) const
{
    throw Exception("VoronoiCoverage::TriIntegral(): shouldn't be called");

    return Vector2D();
}