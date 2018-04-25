/*
  IDSamples.cpp

  add class id to samples with only coordinates

  Li-Yi Wei
  April 25, 2018

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
        cerr << argv[0] << " input_sample_file_name dimension class_id" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_sample_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const int class_id = atoi(argv[++arg_ctr]);

    const CoordinateType default_value = 0;

    // input samples
    const bool has_id = false;
    vector<Sample> input_samples;
    if(! Utility::ReadSamples(has_id, dimension, input_sample_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_sample_file_name << endl;
        return 1;
    }
    
    // process
    for(unsigned int k = 0; k < input_samples.size(); k++)
    {
        input_samples[k].id = class_id;
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
