/*
  DelaunayTest.cpp

  mimic from Lischinski's random point set test

  Li-Yi Wei
  10/31/2007

*/

#include <iostream>
using namespace std;

#include <stdlib.h>

#include "Delaunay.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " num_points" << endl;
        return 1;
    }

    int argCtr = 0;
    const int num_points = atoi(argv[++argCtr]);

    vector<Delaunay::Point2D> points(num_points);

    cerr << "generate " << num_points << " random points" << endl;
    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i].x = Random::UniformRandom();
        points[i].y = Random::UniformRandom();
    }

    cerr << "init Delaunay" << endl;

#if 1
    Delaunay * mesh = new Delaunay(0, 1, 0, 1);
#else
    Delaunay::Point2D p1(-1, -1), p2(2, -1), p3(0.5, 3);
    Delaunay * mesh = new Delaunay(p1, p2, p3);
#endif
    
    cerr << "insert sites into Delaunay" << endl;
    for(unsigned int i = 0; i < points.size(); i++)
    {
        if(!mesh->InsertSite(points[i]))
        {
            cerr << "cannot insert site" << endl;
            return 1;
        }
    }

    cerr << "display Delaunay" << endl;
    vector<const QuadEdge::Edge * > edges;
    mesh->GetEdges(edges);
    for(unsigned int i = 0; i < edges.size(); i++)
    {
        const Delaunay::Point2D & source = Delaunay::GetOrigin(edges[i]);
        const Delaunay::Point2D & destination = Delaunay::GetDestination(edges[i]);
        
        cout << "edge " << i << " : ";
        cout << "(" << source.x << ", " << source.y << ") - ";
        cout << "(" << destination.x << ", " << destination.y << ")" << endl;
    }
    
    cerr << "delete Delaunay" << endl;
    delete mesh;

    cerr << "sanity check" << endl;
    for(unsigned int i = 0; i < points.size(); i++)
    {
        // make sure all points are disconnected from all edges
        if(points[i].NumEdgeRings() > 0)
        {
            cerr << "point " << i << " has lingering edges" << endl;
            return 1;
        }
    }
    
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
