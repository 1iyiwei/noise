/*
  Coverage.cpp

  Li-Yi Wei
  10/24/2013

*/


#include <iostream>
#include <string>
#include <cmath>
using namespace std;

#include <stdlib.h>
#include <stdio.h>

#include "Math.hpp"
#include "Math2D.hpp"
#include "Utility.hpp"

#include "MultiLinearCoverage.hpp"

#include "Random.hpp"
#include "Exception.hpp"

#if 0
Coverage::Point2D RandomPoint(void)
{
    return Coverage::Point2D(Random::UniformRandom(), Random::UniformRandom());
}

int SingleTriAreaTest(int argc, char **argv)
{
    int min_argc = 2;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " threshold_ratio" << endl;

        return 1;
    }

    int arg_ctr = 0;
    const float threshold_ratio = atof(argv[++arg_ctr]);

    Coverage::Polygon polygon;
    polygon.ring = vector<Coverage::Point2D>(3);

    polygon.center = Coverage::Point2D(0.5, sqrt(3.0)/6.0);
    polygon.ring[0] = Coverage::Point2D(0, 0);
    polygon.ring[1] = Coverage::Point2D(1, 0);
    polygon.ring[2] = Coverage::Point2D(0.5, sqrt(3.0)/2.0);
    
    const Coverage::Real truth = fabs(Math2D::Area(polygon.ring[0], polygon.ring[1], polygon.ring[2]));

    const Coverage::Real answer = fabs(Coverage::Integral(polygon, 0.0));

    if(fabs(answer-truth) > fabs(truth*threshold_ratio))
    {
        cerr << "failed: " << "(" << polygon.center.x << ", " << polygon.center.y << ") (" << polygon.ring[0].x << ", " << polygon.ring[0].y << ") (" << polygon.ring[1].x << ", " << polygon.ring[1].y << ") : " << truth << " <-> " << answer << endl;
    }
    else
    {
        cerr << "succeeded!" << endl;
    }

    return 0;
}

int TriAreaTest(int argc, char **argv)
{
    int min_argc = 3;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " num_tests threshold_ratio" << endl;

        return 1;
    }

    int arg_ctr = 0;
    const int num_runs = atoi(argv[++arg_ctr]);
    const float threshold_ratio = atof(argv[++arg_ctr]);

    Random::InitRandomNumberGenerator();

    Coverage::Polygon polygon;
    polygon.ring = vector<Coverage::Point2D>(3);

    int num_fails = 0;
    for(int i = 0; i < num_runs; i++)
    {
        polygon.center = RandomPoint();
        for(unsigned int j = 0; j < polygon.ring.size(); j++)
        {
            polygon.ring[j] = RandomPoint();
        }

        {
            // centroid

            polygon.center = Coverage::Point2D(0, 0);
            
            for(unsigned int j = 0; j < polygon.ring.size(); j++)
            {
                polygon.center.x += polygon.ring[j].x;
                polygon.center.y += polygon.ring[j].y;
            }

            polygon.center.x /= polygon.ring.size();
            polygon.center.y /= polygon.ring.size();
        }

        const Coverage::Real truth = fabs(Math2D::Area(polygon.ring[0], polygon.ring[1], polygon.ring[2]));

        const Coverage::Real answer = fabs(Coverage::Integral(polygon, 0.0));

        if(fabs(answer-truth) > fabs(truth*threshold_ratio))
        {
            cerr << "failed: ";
            num_fails++;
        }
        else
        {
            cerr << "succeeded: ";
        }

        cerr << "(" << polygon.center.x << ", " << polygon.center.y << ") (" << polygon.ring[0].x << ", " << polygon.ring[0].y << ") (" << polygon.ring[1].x << ", " << polygon.ring[1].y << ") : " << truth << " <-> " << answer << endl;
    }

    cerr << num_fails << "/" << num_runs << " failed!" << endl;

    return 0;
}

#endif

int Main(int argc, char **argv)
{
    int min_argc = 8;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name dimension vdpower boundary_condition (0 for none, 1 for toroidal) normalization (0 for false, 1 for true) domain_size (dimension numbers) very_small" << endl;

        return 1;
    }

    int arg_ctr = 0;

    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const string vdpower_string = argv[++arg_ctr];
    const float vdpower = (vdpower_string == "inf" ? Math::INF : atof(vdpower_string.c_str()));

    const Coverage::BoundaryCondition boundary_condition = static_cast<Coverage::BoundaryCondition>(atoi(argv[++arg_ctr]));

    const bool normalization = static_cast<bool>(atoi(argv[++arg_ctr]));

    if(dimension != 2)
    {
        cerr << argv[0] << ": currently only works for 2D" << endl;
        return 1;
    }

    if(argc < (min_argc + dimension - 1))
    {
        cerr << argv[0] << ": not enough input arguments" << endl;
        return 1;
    }

    vector<float> region(4);
    region[2] = 0;
    region[3] = arg_ctr+1 < argc ? atof(argv[++arg_ctr]) : -1;
    region[0] = 0;
    region[1] = arg_ctr+1 < argc ? atof(argv[++arg_ctr]) : -1;
    
    const double very_small = atof(argv[++arg_ctr]);

    // samples
    vector<Sample> samples;
    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << argv[0] << ": cannot read samples from " << input_file_name << endl;
        return 1;
    }

    vector<Coverage::Point2D> points(samples.size());

    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i].id = i;
        points[i].x = samples[i].coordinate[1];
        points[i].y = samples[i].coordinate[0];
    }

    // compute
    Random::InitRandomNumberGenerator();

    MultiLinearCoverage coverage;

    const Coverage::Real ideal_value = (normalization ? coverage.Mintegral(region[0], region[1], region[2], region[3]/*, very_small, boundary_condition*/, points.size(), vdpower) : 1);

    const Coverage::Real real_value = coverage.Integral(region[0], region[1], region[2], region[3], very_small, boundary_condition, points, vdpower);

    cout << real_value/ideal_value << endl;

    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        // return SingleTriAreaTest(argc, argv);
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr << "Error : " << e.Message() <<endl;
        return 1;
    }
}