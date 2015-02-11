/*
  Math.cpp

  Li-Yi Wei
  09/04/2013

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Math.hpp"
#include "Exception.hpp"
#include "Random.hpp"
#include "Sample.hpp"

// assumes unit box domain
int RValue(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " dimension rho_value num_samples" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const int dimension = atoi(argv[++arg_ctr]);
    const float rho_value = atof(argv[++arg_ctr]);
    const float num_samples = atof(argv[++arg_ctr]);

    cout << Math::ComputeMaxMinDistance(dimension, num_samples)*rho_value << endl;

    return 0;
}

int Rho(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " dimension r_input num_samples" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const int dimension = atoi(argv[++arg_ctr]);
    const float r_input = atof(argv[++arg_ctr]);
    const float num_samples = atof(argv[++arg_ctr]);

    // cout << "MaxMinDistance(" << dimension << ", " << num_samples << ") == " << Math::ComputeMaxMinDistance(dimension, num_samples) << endl;
    cout << r_input/Math::ComputeMaxMinDistance(dimension, num_samples) << endl;

    return 0;
}

int SampleNum(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " dimension r_input rho_value" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const int dimension = atoi(argv[++arg_ctr]);
    const float r_input = atof(argv[++arg_ctr]);
    const float rho_value = atof(argv[++arg_ctr]);

    cout << Math::ComputeMaxNumSamples(dimension, r_input/rho_value) << endl;

    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        if(argc < 2)
        {
            cerr << "Usage: " << argv[0] << " option" << endl;
            return 1;
        }

        int arg_ctr = 0;
        const string option = argv[++arg_ctr];

        // return SFT1DUniformRandom(argc, argv);
        // return MeanCosUniformRandom(argc, argv);
        if(option == "rvalue")
        {
            return RValue(argc-1, &argv[1]);
        }
        else if(option == "rho")
        {
            return Rho(argc-1, &argv[1]);
        }
        else if(option == "samplenum")
        {
            return SampleNum(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown math option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
