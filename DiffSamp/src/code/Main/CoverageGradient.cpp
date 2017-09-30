/*
  CoverageGradient.cpp

  Chia-Kai Liang
  3/31/2014

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
#include "LloydRelaxation.hpp"

#include "Random.hpp"
#include "Exception.hpp"

typedef LloydRelaxation::VoronoiRegion Polygon;
typedef Sample GradientVector;

int Main(int argc, char **argv)
{
    const int xIdx = 1;
    const int yIdx = 1 - xIdx;

    int min_argc = 7;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name dimension vdpower boundary_condition (0 for none, 1 for toroidal) domain_size (dimension numbers) very_small [output_file_name]" << endl;
        return 1;
    }

    int arg_ctr = 0;

    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const string vdpower_string = argv[++arg_ctr];
    const float vdpower = (vdpower_string == "inf" ? Math::INF : atof(vdpower_string.c_str()));

    const Coverage::BoundaryCondition boundary_condition = static_cast<Coverage::BoundaryCondition>(atoi(argv[++arg_ctr]));

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

    // output and init file names
    string output_file_name;

    if((arg_ctr + 1) < argc)
    {
        output_file_name = argv[++arg_ctr];
    }
    else
    {
        output_file_name = "cout";
    }


    // load samples
    vector<Sample> samples;
    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << argv[0] << ": cannot read samples from " << input_file_name << endl;
        return 1;
    }

    // compute
    Random::InitRandomNumberGenerator();

    MultiLinearCoverage coverage;

    // create the Voronoi diagram within the ROI and associate each Voronoi cell with the source 4D sample
    size_t sampleSize = samples.size();

    vector<LloydRelaxation::Point2D> points(sampleSize);
    vector<const LloydRelaxation::Point2D *> pointsPtrs(sampleSize);

    for(size_t i = 0; i < points.size(); i++)
    {
        points[i].id = (int)i;
        points[i].x = samples[i].coordinate[xIdx];
        points[i].y = samples[i].coordinate[yIdx];
        pointsPtrs[i] = & points[i];
    }

    vector<Polygon> polygons;
    polygons.reserve(sampleSize);
    const string message = LloydRelaxation::Voronoi(region[0], region[1], region[2], region[3], very_small, boundary_condition, pointsPtrs, polygons);
    assert(polygons.size() == samples.size());

    vector<GradientVector> gradients(samples.size());
    for (size_t i = 0; i < gradients.size(); ++i)
    {
        const Coverage::Vector2D gradient = coverage.EnergyFunctionGradient(polygons[i], vdpower);
        GradientVector v(2);
        v.id = 0;
        v.coordinate[xIdx] = gradient.x;
        v.coordinate[yIdx] = gradient.y;
        gradients[i] = v;
    }

    // write to output
    if(! Utility::WriteSamples(gradients, output_file_name))
    {
        cerr << "cannot write samples to " << output_file_name << endl;
        return 1;
    }


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