/*
  DrawEnergySampleTracker.cpp

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
    const int argc_min = 6;
    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " init_sample_file_name (negative class id for non-sample points) dimension domain_size (dimension numbers) trial_class num_trials" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);

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

    const int trial_class = atoi(argv[++arg_ctr]);
    const int num_trials = atoi(argv[++arg_ctr]);

    // init samples
    vector<Sample> points_and_samples;
    if(! Utility::ReadSamples(dimension, input_file_name, points_and_samples))
    {
        cerr << "cannot read samples from " << input_file_name << endl;
        return 1;
    }

    // extract samples from points
    vector<Sample> samples;
    {
        for(unsigned int i = 0; i < points_and_samples.size(); i++)
        {
            if(points_and_samples[i].id >= 0)
            {
                samples.push_back(points_and_samples[i]);
            }
        }
    }

    // counting
    vector<int> num_samples;    
    if(! Utility::CountSamples(samples, num_samples))
    {
        cerr << "error in counting samples" << endl;
        return 1;
    }

    const int num_class = num_samples.size();

    // energy samples
    vector<EnergySample> energy_samples(samples.size());

    for(unsigned int i = 0; i < energy_samples.size(); i++)
    {
        if(!energy_samples[i].SetSample(samples[i]))
        {
            throw Exception("cannot set sample");
        }
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

    // set energy samples
    vector<const EnergySample *> p_energy_samples(energy_samples.size());
    for(unsigned int i = 0; i < p_energy_samples.size(); i++)
    {
        p_energy_samples[i] = &energy_samples[i];
    }
    
    if(! energy_sample_tracker.Set(p_energy_samples))
    {
        throw Exception("cannot set energy samples");
    }

    // init random
    Random::InitRandomNumberGenerator();

    // trials
    vector<Sample> trials(num_trials);
    for(unsigned int i = 0; i < trials.size(); i++)
    {
        Sample & sample = trials[i];
        
        sample.id = (trial_class >= 0 ? trial_class : static_cast<int>(floor(Random::UniformRandom()*num_class))%num_class);

        if(! energy_sample_tracker.Trough(sample))
        {
            throw Exception("cannot get trough");
        }
    }

    // output    
    cerr << trials.size() << " trials" << endl;
    if(!Utility::WriteSamples(trials, "cout"))
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
