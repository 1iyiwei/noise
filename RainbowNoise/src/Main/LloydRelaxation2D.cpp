/*
  LloydRelaxation2D.cpp

  Li-Yi Wei
  11/01/2007
  05/02/2008 (modified for RainbowNoise)

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Sample.hpp"
#include "LloydRelaxation.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    if(argc < 6)
    {
        cerr << "Usage: " << argv[0] << " input_file_name dimension domain_size (dimension numbers) num_iterations" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    if(dimension != 2)
    {
        cerr << "relaxation only works for 2D now" << endl;
        return 1;
    }
    const float x_min = 0; const float x_max = atof(argv[++argCtr]);
    const float y_min = 0; const float y_max = atof(argv[++argCtr]);
    const int num_iterations = atoi(argv[++argCtr]);
    const LloydRelaxation::BoundaryCondition boundary_condition = DelaunayMesh::TOROIDAL;

    // read in points
    ifstream input(input_file_name); // read in samples
    
    vector<Sample> samples;
    
    if(!input)
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
            input >> sample.id;

            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    cerr << "total " << samples.size() << " samples read" << endl;

    // init points
    vector< vector<LloydRelaxation::Point2D *> > points;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        while(points.size() <= samples[i].id)
        {
            vector<LloydRelaxation::Point2D *> empty;
            points.push_back(empty);
        }

        if((samples[i].id < 0) || (samples[i].id >= points.size()))
        {
            cerr << "weird: " << samples[i].id << endl;
            return 1;
        }

        LloydRelaxation::Point2D *baby = new LloydRelaxation::Point2D(samples[i].coordinate[0], samples[i].coordinate[1]);

        points[samples[i].id].push_back(baby);
    }

    vector<LloydRelaxation::Point2D *> all_points;
    for(unsigned int i = 0; i < points.size(); i++)
        for(unsigned int j = 0; j < points[i].size(); j++)
        {
            all_points.push_back(points[i][j]);
        }

    points.push_back(all_points);

    // relaxation
    cerr << "relaxation";
    for(unsigned k = 0; k < num_iterations; k++)
    {
        cerr << ".";

        for(unsigned i = 0; i < points.size(); i++)
        {
            const string result = LloydRelaxation::RunOnce(x_min, x_max, y_min, y_max, boundary_condition, points[i]);
        
            if(result != "")
            {
                throw Exception(result.c_str());
            }
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
    cerr << "output result" << endl;
    for(unsigned int i = 0; i < points.size()-1; i++)
        for(unsigned int j = 0; j < points[i].size(); j++)
        {
            cout << i << " " << points[i][j]->x << " " << points[i][j]->y << endl;
        }

    // clean up
    for(unsigned int i = 0; i < all_points.size(); i++)
    {
        delete all_points[i];
    }
    points.clear(); all_points.clear();
    
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
