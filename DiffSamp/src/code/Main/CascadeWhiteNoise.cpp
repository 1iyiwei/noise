/*
  CascadeWhiteNoise.cpp

  experimental white noise generated in a cascade fashion
  (for anisotropic blue noise project)

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
#include "Random.hpp"
#include "UniformWhiteNoise.hpp"
#include "Math.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 6;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " dimension num_samples domain_size (dimension numbers) sub_domain_scale (scalar, relative to domain_size) boundary_condition (0 for none, 1 for toroidal)" << endl;
        return 1;
    }

    int arg_ctr = 0;

    const int dimension = atoi(argv[++arg_ctr]);
    const int num_samples = atoi(argv[++arg_ctr]);
    
    if(argc < (argc_min + dimension -1))
    {
        cerr << "insufficient number of input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }
    
    const float sub_domain_scale = atof(argv[++arg_ctr]);

    vector<float> sub_domain_size(domain_size);
    for(unsigned int i = 0; i < sub_domain_size.size(); i++)
    {
        sub_domain_size[i] = domain_size[i]*sub_domain_scale;
    }

    const int boundary_condition = atoi(argv[++arg_ctr]);

    // init
    Random::InitRandomNumberGenerator();

    UniformWhiteNoise domain_white_noise(domain_size);
    UniformWhiteNoise sub_domain_white_noise(sub_domain_size);

    // generation
    vector<Sample> samples;
    Sample anchor(dimension), sample(dimension);

    while(samples.size() < num_samples)
    {
        if(! domain_white_noise.Get(anchor)) throw Exception("cannot get anchor");
        if(! sub_domain_white_noise.Get(sample)) throw Exception("cannot get sample");

        for(int i = 0; i < sample.coordinate.Dimension(); i++)
        {
            sample.coordinate[i] += anchor.coordinate[i] - sub_domain_size[i]*0.5;
        }

        if(boundary_condition == 1) // toroidal
        {
            for(int i = 0; i < sample.coordinate.Dimension(); i++)
            {
                sample.coordinate[i] = Math::mod(sample.coordinate[i], domain_size[i]);
            }
        }

        if(Utility::Inside(domain_size, sample))
        {
            samples.push_back(sample);
        }
    }

    // output
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
