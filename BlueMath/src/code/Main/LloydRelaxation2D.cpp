/*
  LloydRelaxation2D.cpp

  Li-Yi Wei
  11/01/2007
  05/02/2008 (modified for RainbowNoise)
  10/26/2013 (modified for LightSamp)

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Sample.hpp"
#include "LloydRelaxation.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    int min_argc = 7;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_file_name dimension boundary_condition (0 for none, 1 for toroidal) domain_size (dimension numbers) num_iterations" << endl;

        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    if(dimension != 2)
    {
        cerr << argv[0] << " only works for 2D now" << endl;
        return 1;
    }

    const LloydRelaxation::BoundaryCondition boundary_condition = static_cast<LloydRelaxation::BoundaryCondition>(atoi(argv[++arg_ctr]));

    const float x_min = 0; const float x_max = atof(argv[++arg_ctr]);
    const float y_min = 0; const float y_max = atof(argv[++arg_ctr]);
    const int num_iterations = atoi(argv[++arg_ctr]);

    // read in points
    vector<Sample> samples;
    
    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << argv[0] << ": cannot read samples from " << input_file_name << endl;
        return 1;
    }
    
    cerr << "total " << samples.size() << " samples read" << endl;

    // init points
    vector<LloydRelaxation::Point2D *> points;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        LloydRelaxation::Point2D *baby = new LloydRelaxation::Point2D(samples[i].coordinate[0], samples[i].coordinate[1]);

        points.push_back(baby);
    }

    // relaxation
    cerr << "relaxation";
    for(int k = 0; k < num_iterations; k++)
    {
        cerr << ".";

        const string result = LloydRelaxation::RunOnce(x_min, x_max, y_min, y_max, boundary_condition, points);
        
        if(result != "")
        {
            throw Exception(result.c_str());
        }

#if 0
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
#endif
    }
    cerr << endl;
    
    // output
    if(samples.size() != points.size())
    {
        cerr << argv[0] << " samples.size() != points.size()" << endl;
        return 1;
    }

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        samples[i].coordinate[0] = points[i]->x;
        samples[i].coordinate[1] = points[i]->y;
    }

    if(! Utility::WriteSamples(samples, "cout"))
    {
        cerr << argv[0] << ": cannot write samples" << endl;
        return 1;
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
