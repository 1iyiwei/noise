/*
  ApplyAperture.cpp

  Chia-Kai Liang
  09/04/2013

*/

#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <cfloat>
using namespace std;

#include <stdlib.h>

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Exception.hpp"
#include "ApertureDomain.hpp"

ApertureType DecodeApertureType(std::string aperture_type_str)
{
    ApertureType type = RectangularAperture;
    if(aperture_type_str.find("circular") != string::npos)
    {
        type = CircularAperture;
    }
    else if(aperture_type_str.find("hexagonal") != string::npos)
    {
        type = HexagonalAperture;
    }
    else if(aperture_type_str.find("rectangular") != string::npos)
    {
        type = RectangularAperture;
    }
    return type;
}

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 5;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_sample_file_name dimension domain aperture_type" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_sample_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);

    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }

    // create the aperture
    float minDomain = FLT_MAX;
    if (dimension == 2)
    {
        minDomain = min(domain_size[0], domain_size[1]);
    }
    else if (dimension == 4)
    {
        minDomain = min(domain_size[2], domain_size[3]);
    }

    const std::string aperture_type_str = argv[++arg_ctr];
    ApertureType aperture_type = DecodeApertureType(aperture_type_str);
    ApertureDomain aperture(minDomain*0.5f, aperture_type);

    // input samples
    vector<Sample> input_samples;
    if(! Utility::ReadSamples(dimension, input_sample_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_sample_file_name << endl;
        return 1;
    }
    
    // process
    int rejectedCount = 0;
    vector<Sample> output_samples;
    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        Sample & input_sample = input_samples[i];
        Sample output_sample(input_sample);

        if(aperture.Inside(input_sample))
        {
            output_samples.push_back(output_sample);
        }
        else
        {
            rejectedCount++;
        }
    }

    // output
    if(! Utility::WriteSamples(output_samples, "cout"))
    {
        cerr << "cannot write samples to " << "cout" << endl;
        return 1;
    }

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
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
