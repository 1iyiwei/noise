/*
  LloydRelaxation2DRandom.cpp

  Li-Yi Wei
  11/01/2007
  05/02/2008 (modified for RainbowNoise)

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "LloydRelaxation.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    Random::InitRandomNumberGenerator();
    
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " num_points num_iterations boundary_condition (0 for none, 1 for toroidal)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int num_points = atoi(input_file_name);
    const int num_iterations = atoi(argv[++argCtr]);
    const LloydRelaxation::BoundaryCondition boundary_condition = static_cast<LloydRelaxation::BoundaryCondition>(atoi(argv[++argCtr]));

    cerr << "init points" << endl;
    vector<LloydRelaxation::Point2D *> points(num_points);

    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i] = new LloydRelaxation::Point2D(Random::UniformRandom(), Random::UniformRandom());
    }

    const float x_min = 0; const float x_max = 1;
    const float y_min = 0; const float y_max = 1;

    // relaxation
    cerr << "relaxation";
    for(unsigned k = 0; k < num_iterations; k++)
    {
        cerr << ".";

        const string result = LloydRelaxation::RunOnce(x_min, x_max, y_min, y_max, boundary_condition, points);
        
        if(result != "")
        {
            throw Exception(result.c_str());
        }
        
        // check 
        for(unsigned int i = 0; i < points.size(); i++)
        {
            const LloydRelaxation::Point2D & point = *points[i];

            if((point.x < x_min) || (point.x > x_max) || (point.y < y_min) || (point.y > y_max))
            {
                cerr << "out of range point: " << point.x << ", " << point.y << endl;
                return 1;
            }
            
            if(point.NumEdgeRings() > 0)
            {
                cerr << "points[" << i << "] not cleaned of edges" << endl;
                return 1;
            }
        }
    }
    cerr << endl;
    
    // output
    cerr << "output result" << endl;
    for(unsigned int i = 0; i < points.size(); i++)
    {
        cout << points[i]->x << " " << points[i]->y << endl;
    }

    // clean up
    for(unsigned int i = 0; i < points.size(); i++)
    {
        delete points[i];
    }
    points.clear();
    
    // done
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
