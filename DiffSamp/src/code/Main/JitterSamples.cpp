/*
  JitterSamples.cpp

  Li-Yi Wei
  11/01/2013

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>

#include "Random.hpp"
#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

string Jitter(const string type, const vector<string> & params, Sample & sample)
{
    string message;

    if(type == "gaussian")
    {
        if(params.size() <= 0)
        {
            message = "need gaussian jitter amount";
        }
        else
        {
            const float amount = atof(params[0].c_str());
            
            for(int k = 0; k < sample.coordinate.Dimension(); k++)
            {
                sample.coordinate[k] += Random::GaussianRandom()*amount;
            }
        }
    }
    else
    {
        message = "unknown jitter type";
    }

    return message;
}

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 5;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_sample_file_name dimension jitter_type jitter_params " << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_sample_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const string jitter_type = argv[++arg_ctr];
    const vector<string> jitter_params = Utility::Convert(argc-arg_ctr-1, &argv[arg_ctr+1]);

    // input samples
    vector<Sample> input_samples;
    if(! Utility::ReadSamples(dimension, input_sample_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_sample_file_name << endl;
        return 1;
    }
    
    // process
    Random::InitRandomNumberGenerator();

    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        Sample & sample = input_samples[i];

        const string message = Jitter(jitter_type, jitter_params, sample);

        if(message != "")
        {
            cerr << "error in jitter sample: " << message << endl;
            return 1;
        }
    }

    // output
    if(! Utility::WriteSamples(input_samples, "cout"))
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
