/*
  LloydRelaxationRandom.cpp

  Li-Yi Wei
  11/01/2007

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "LloydRelaxation.hpp"
#include "Random.hpp"
#include "Exception.hpp"

template <class T>
T mod(const T a, const T b)
{
    return (a-floor(a/b)*b);
}

LloydRelaxation::Real mymod(const LloydRelaxation::Real a, const LloydRelaxation::Real b)
{
    return mod<LloydRelaxation::Real>(a, b);
}

LloydRelaxation::Point2D Jitter(const LloydRelaxation::Point2D & input, const LloydRelaxation::Point2D & jitter, const LloydRelaxation::Real x_min, const LloydRelaxation::Real x_max, const LloydRelaxation::Real y_min, const LloydRelaxation::Real y_max)
{
    LloydRelaxation::Point2D result(mymod(input.x - x_min + jitter.x, x_max-x_min) + x_min, mymod(input.y - y_min + jitter.y, y_max-y_min) + y_min);

    if((result.x < x_min) || (result.x > x_max) || (result.y < y_min) || (result.y > y_max))
    {
        cerr << "mymod(" << input.x - x_min + jitter.x << ", " << x_max-x_min << ") = " << mymod(input.x - x_min + jitter.x, x_max-x_min) << endl;
        cerr << "mymod(" << input.y - y_min + jitter.y << ", " << y_max-y_min << ") = " << mymod(input.y - y_min + jitter.y, y_max-y_min) << endl;
        cerr << "mymod(" << input.x << "-" << x_min << "+" << jitter.x << ", " << x_max << "-" << x_min << ")+" << x_min << " = " << mymod(input.x - x_min + jitter.x, x_max-x_min)+x_min << endl;
        cerr << "mymod(" << input.y << "-" << y_min << "+" << jitter.y << ", " << y_max << "-" << y_min << ")+" << y_min << " = " << mymod(input.y - y_min + jitter.y, y_max-y_min)+y_min << endl;
        cerr << "input (" << input.x << ", " << input.y << "), jitter (" << jitter.x << ", " << jitter.y << "), jittered (" << result.x << ", " << result.y << ")" << endl;
        throw Exception("jittered point out of bound");
    }

    return result;
}

int Main(int argc, char **argv)
{
    Random::InitRandomNumberGenerator();
    
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " num_points num_iterations toroidal_jitter_amount" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int num_points = atoi(input_file_name);
    const int num_iterations = atoi(argv[++argCtr]);
    const float toroidal_jitter_amount = atof(argv[++argCtr]);
    
    cerr << "init points" << endl;
    vector<LloydRelaxation::Point2D *> points(num_points);

    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i] = new LloydRelaxation::Point2D(Random::UniformRandom(), Random::UniformRandom());
    }

    const float x_min = 0; const float x_max = 1;
    const float y_min = 0; const float y_max = 1;

    const LloydRelaxation::Point2D jitter(toroidal_jitter_amount*Random::UniformRandom()*(x_max - x_min), toroidal_jitter_amount*Random::UniformRandom()*(y_max - y_min));
    
    // relaxation
    cerr << "relaxation";
    for(unsigned k = 0; k < num_iterations; k++)
    {
        cerr << ".";

        for(unsigned int i = 0; i < points.size(); i++)
        {
            *points[i] = Jitter(*points[i], jitter, x_min, x_max, y_min, y_max);
        }
        
        const string result = LloydRelaxation::RunOnce(x_min, x_max, y_min, y_max, points);
        
        for(unsigned int i = 0; i < points.size(); i++)
        {
            *points[i] = Jitter(*points[i], LloydRelaxation::Point2D(-jitter.x, -jitter.y), x_min, x_max, y_min, y_max);
        }
        
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
