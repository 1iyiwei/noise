/*
  RandomPickPointsSites.cpp

  from a given sample set, 
  randomly classify them into multiple classes of samples plus points

  Li-Yi Wei
  07/19/2009

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

#include "Sample.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    const int argc_min = 5;
    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " sample_file_name dimension num_class num_samples_per_class (num_class integers)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const int num_class = atoi(argv[++arg_ctr]);

    if(argc < (argc_min + num_class - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<int> num_samples(num_class, 0);
    for(unsigned int i = 0; i < num_samples.size(); i++)
    {
        num_samples[i] = atoi(argv[++arg_ctr]);
    }

    // read samples
    vector<Sample> samples;
    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << "cannot read samples from " << input_file_name << endl;
        return 1;
    }

    // check
    int total_num_samples = 0;
    for(unsigned int i = 0; i < num_samples.size(); i++)
    {
#if 0
        if(num_samples[i] <= 0)
        {
            cerr << "num_samples should be > 0" << endl;
            return 1;
        }
#endif
        total_num_samples += num_samples[i];
    }

    const int num_points = samples.size() - total_num_samples;
    if(num_points < 0)
    {
        cerr << "num_points should be >= 0" << endl;
        return 1;
    }

    // init all to points
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        samples[i].id = -1;
    }

    // init random
    Random::InitRandomNumberGenerator();

    // do the work
    vector<int> remaining_index(samples.size());
    for(unsigned int i = 0; i < remaining_index.size(); i++)
    {
        remaining_index[i] = i;
    }

    for(unsigned int i = 0; i < num_samples.size(); i++)
    {
        for(unsigned int j = 0; j < num_samples[i]; j++)
        {
            if(remaining_index.size() <= 0) throw Exception("weird");

            const int remaining_entry = static_cast<int>(Random::UniformRandom()*remaining_index.size())%remaining_index.size();
            const int selection = remaining_index[remaining_entry];

            remaining_index[remaining_entry] = remaining_index[remaining_index.size()-1];
            remaining_index.pop_back();

            samples[selection].id = i;
        }
    }

    // output
    if(!Utility::WriteSamples(samples, "cout"))
    {
        cerr << "cannot write out points" << endl;
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
