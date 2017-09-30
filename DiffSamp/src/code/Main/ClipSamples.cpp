/*
  ClipSamples.cpp

  clip away samples outside domain

  Li-Yi Wei
  10/16/2008

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 4;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_sample_file_name dimension domain_size (dimension numbers) " << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_sample_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);

    if(argc < (min_argc + dimension -1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }

    // input samples
    vector<Sample> input_samples;
    if(! Utility::ReadSamples(dimension, input_sample_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_sample_file_name << endl;
        return 1;
    }
    
    // process
    vector<Sample> output_samples;

    vector<float> domain_min(domain_size.size(), 0.0);
    
    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        if(Utility::Inside(domain_min, domain_size, input_samples[i]))
        {
            output_samples.push_back(input_samples[i]);
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
