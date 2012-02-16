/*
  MakeRandom.cpp
  
  make a random importance field

  Li-Yi Wei
  10/01/2008

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "Random.hpp"

int Main(int argc, char **argv)
{
    const int min_argc = 4;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension output_size (pixels per unit domain size) output_file_name.pfm domain_size (optional, dimension numbers)" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int output_size_1d = atoi(argv[++argCtr]);
    const char * output_file_name = argv[++argCtr];
    
    // domain size
    vector<float> domain_size(dimension, 1.0);

    if((argCtr + dimension) < argc)
    {
        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            domain_size[i] = atof(argv[++argCtr]);
        }

        cerr << "domain size:";
        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            cerr << " " << domain_size[i];
        }
        cerr << endl;
    }

    vector<int> output_size(dimension, output_size_1d);
    for(unsigned int i = 0; i < output_size.size(); i++)
    {
        output_size[i] = static_cast<int>(floor(output_size_1d*domain_size[i]+0.5));
    }

    // work
    const unsigned long random_seed = time(0);
    Random::InitRandomNumberGenerator(random_seed);

    Array<FrameBuffer::PF> output(output_size);
    vector<FrameBuffer::PF> pixels;
    output.Get(pixels);
    for(unsigned int i = 0; i < pixels.size(); i++)
    {
        pixels[i].r = Random::UniformRandom();
        pixels[i].g = Random::UniformRandom();
        pixels[i].b = Random::UniformRandom();
    }
    output.Put(pixels);
    
    if(! FrameBuffer::WritePFM(output, output_file_name))
    {
        cerr << "cannot write " << output_file_name << endl;
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
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
