/*
  WhiteNoiseSamples.cpp

  produce uniform random samples in a domain

  Li-Yi Wei
  07/03/2009

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

#include "Utility.hpp"
// #include "Math.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 5;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " dimension num_class num_samples_per_class (num_class integers) domain_size (dimension numbers)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const int dimension = atoi(argv[++arg_ctr]);
    const int num_class = atoi(argv[++arg_ctr]);

    if(argc < (argc_min + num_class - 1 + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<int> num_samples(num_class);
    for(unsigned int i = 0; i < num_samples.size(); i++)
    {
        num_samples[i] = atoi(argv[++arg_ctr]);
    }

    vector<float> domain_size(dimension, 1.0);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }
    
    // init random
    Random::InitRandomNumberGenerator();

    // init samples
    vector<Sample> samples;
    Sample sample(dimension);

    for(unsigned int which_class = 0; which_class < num_samples.size(); which_class++)
    {
        for(int which_sample = 0; which_sample < num_samples[which_class]; which_sample++)
        {
            for(int i = 0; i < sample.coordinate.Dimension(); i++)
            {
                sample.coordinate[i] = Random::UniformRandom()*domain_size[i];
            }
            
            sample.id = which_class;
            sample.value = 1.0;
            
            samples.push_back(sample);
        }
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
