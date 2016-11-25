/*
  ShiftSamples.cpp

  Li-Yi Wei
  05/08/2011

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
        cerr << argv[0] << " input_sample_file_name dimension shift_amount " << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_sample_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    if(argc < (min_argc + dimension -1))
    {
        cerr << "insufficient arguments for shift amount" << endl;
        return 1;
    }
    vector<float> shift_amount(dimension);
    for(unsigned int i = 0; i < shift_amount.size(); i++)
    {
        shift_amount[i] = atof(argv[++arg_ctr]);
    }

    // input samples
    vector<Sample> input_samples;
    if(! Utility::ReadSamples(dimension, input_sample_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_sample_file_name << endl;
        return 1;
    }
    
    // process
    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        Sample & sample = input_samples[i];

        for(int j = 0; j < sample.coordinate.Dimension(); j++)
        {
            sample.coordinate[j] += shift_amount[j];
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
