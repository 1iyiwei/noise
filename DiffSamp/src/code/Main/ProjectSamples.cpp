/*
  ProjectSamples.cpp

  Li-Yi Wei
  09/04/2013

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

bool ProjectSample(const Sample & input, const vector<int> & projection_dimensions, const CoordinateType default_value, Sample & output)
{
    if(output.coordinate.Dimension() != projection_dimensions.size())
    {
        output = Sample(projection_dimensions.size());
    }

    output.id = input.id;
    output.value = input.value;
    output.r = input.r;

    for(int i = 0; i < output.coordinate.Dimension(); i++)
    {
        const int select = projection_dimensions[i];

        const CoordinateType value = ((select >= 0) && (select < input.coordinate.Dimension())) ? input.coordinate[select] : default_value;

        output.coordinate[i] = value;
    }

    return true;
}

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 4;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_sample_file_name dimension [selected dimensions]" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_sample_file_name = argv[++arg_ctr];
    const int input_dimension = atoi(argv[++arg_ctr]);
    const int output_dimension = argc - (min_argc - 1);

    vector<int> projection_dimensions(output_dimension);
    for(unsigned int i = 0; i < projection_dimensions.size(); i++)
    {
        projection_dimensions[i] = atoi(argv[++arg_ctr]);
#if 0
        if((projection_dimensions[i] < 0) || (projection_dimensions[i] >= input_dimension))
        {
            cerr << "illegal projection dimension" << endl;
            return 1;
        }
#endif
    }

    const CoordinateType default_value = 0;

    // input samples
    vector<Sample> input_samples;
    if(! Utility::ReadSamples(input_dimension, input_sample_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_sample_file_name << endl;
        return 1;
    }
    
    // process
    vector<Sample> output_samples;
    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        Sample & input_sample = input_samples[i];
        Sample output_sample(output_dimension);

        if(! ProjectSample(input_sample, projection_dimensions, default_value, output_sample))
        {
            cerr << "error in project sample" << endl;
            return 1;
        }

        output_samples.push_back(output_sample);
    }

    // output
    if(! Utility::WriteSamples(output_samples, "cout"))
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
