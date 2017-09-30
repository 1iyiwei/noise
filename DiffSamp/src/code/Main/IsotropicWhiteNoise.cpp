/*
  IsotropicWhiteNoise.cpp

  use rejection method to generate general isotropic adaptive white noise

  Li-Yi Wei
  09/01/2010

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <memory>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "IsotropicWhiteNoise.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 4;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " importance_file_name (f2m) num_samples domain_size (dimension numbers)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string importance_file_name = argv[++arg_ctr];
    Array<float> importance;
    if(! FrameBuffer::ReadF2M(importance_file_name, importance))
    {
        cerr << "cannot read from " << importance_file_name << endl;
        return 1;
    }
    
    const int dimension = importance.Dimension();

    const int num_samples = atoi(argv[++arg_ctr]);

    if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension, 1.0);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }
    
    // init random
    Random::InitRandomNumberGenerator();

    IsotropicWhiteNoise white_noise(domain_size, importance);

    // samples
    vector<Sample> samples;
    Sample sample(dimension);

    for(int which_sample = 0; which_sample < num_samples; which_sample++)
    {
        if(! white_noise.Get(sample))
        {
            cerr << "cannot get adaptive white noise sample" << endl;
            return 1;
        }
        
        samples.push_back(sample);
    }

    // output
    string output_file_name;

    if((arg_ctr + 1) < argc)
    {
        output_file_name = argv[++arg_ctr];
    }
    else
    {
        output_file_name = "cout";
    }

    if(! Utility::WriteSamples(samples, output_file_name))
    {
        cerr << "cannot write samples to " << output_file_name << endl;
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
