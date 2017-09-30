/*
  UniformWhiteNoise.cpp

  use rejection method to generate general isotropic uniform white noise

  Li-Yi Wei
  09/06/2010

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
#include "UniformWhiteNoise.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 4;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " dimension num_samples domain_size (dimension numbers)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    
    const int dimension = atoi(argv[++arg_ctr]);

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

    UniformWhiteNoise white_noise(domain_size);

    // samples
    vector<Sample> samples;
    Sample sample(dimension);

    for(int which_sample = 0; which_sample < num_samples; which_sample++)
    {
        if(! white_noise.Get(sample))
        {
            cerr << "cannot get uniform white noise sample" << endl;
            return 1;
        }
        
        samples.push_back(sample);
    }

    // output
    // cerr << samples.size() << " samples" << endl;
    if(!Utility::WriteSamples(samples, "cout"))
    {
        cerr << "cannot write out samples" << endl;
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
