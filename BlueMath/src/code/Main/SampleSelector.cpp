/*
  SampleSelector.cpp

  select samples with specific ids

  Li-Yi Wei
  04/27/2008

  11/02/2010 
  update input mode

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Utility.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    const int num_fixed_arguments = 4;
    if(argc < num_fixed_arguments)
    {
        cerr << "Usage: " << argv[0] << " input-file-name dimension output_file_name group-ids (multiple)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const string output_file_name = argv[++arg_ctr];

    vector<int> ids;
    while((arg_ctr + 1) < argc)
    {
        ids.push_back(atoi(argv[++arg_ctr]));
    }

    vector<Sample> input_samples;
    
    if(! Utility::ReadSamples(dimension, input_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_file_name << endl;
        return 1;
    }

    cerr << "total " << input_samples.size() << " samples read" << endl;
    
    // process
    vector<Sample> output_samples;
    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        for(unsigned int j = 0; j < ids.size(); j++)
        {
            if(input_samples[i].id == ids[j])
            {
                output_samples.push_back(input_samples[i]);
            }
        }
    }

    // output
    const Utility::SampleMode output_mode = (((output_file_name.find(".txt") == string::npos) && (output_file_name != "cout")) ? Utility::SAMPLE_BINARY : Utility::SAMPLE_ASCII);

    if(! Utility::WriteSamples(output_mode, 0, output_samples, output_file_name))
    {
        cerr << "cannot write samples to " << output_file_name << endl;
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
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
