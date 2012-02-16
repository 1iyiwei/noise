/*
  GridNoiseSamples.cpp

  Li-Yi Wei
  09/05/2009

*/


#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <memory>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "ShuffleTools.hpp"
#include "Utility.hpp"
#include "Math.hpp"
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
    
    // energy sample tracker
    const int scale = 1;
    UnionEnergySampleTracker * p_energy_sample_tracker = ShuffleTools::BuildEnergySampleTracker(domain_size, num_samples, scale);

    if(!p_energy_sample_tracker)
    {
        throw Exception("null energy sample tracker");
    }
     
    const auto_ptr<EnergySampleTracker> auto_ptr_energy_sample_tracker(p_energy_sample_tracker);

    EnergySampleTracker & energy_sample_tracker = *auto_ptr_energy_sample_tracker;

    // energy samples
    EnergySample energy_sample;
    Sample sample(dimension);
    vector<EnergySample> energy_samples;
    
    for(unsigned int i = 0; i < num_samples.size(); i++)
    {
        sample.id = i;
        if(!energy_sample.SetSample(sample)) throw Exception("cannot set sample");

        for(int j = 0; j < num_samples[i]; j++)
        {
            energy_samples.push_back(energy_sample);
        }
    }

    // init random
    Random::InitRandomNumberGenerator();

    // trials
    for(unsigned int i = 0; i < energy_samples.size(); i++)
    {
        if(! energy_samples[i].GetSample(sample)) throw Exception("cannot get sample");

        if(! energy_sample_tracker.Trough(sample)) throw Exception("cannot find trough");

        if(! energy_samples[i].SetSample(sample)) throw Exception("cannot set sample");

        if(! energy_sample_tracker.Plus(energy_samples[i])) throw Exception("cannot plus energy sample");
    }

    // output
    vector<Sample> samples(energy_samples.size());
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(! energy_samples[i].GetSample(samples[i]))
        {
            throw Exception("cannot get sample");
        }
    }

    cerr << samples.size() << " samples" << endl;
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
