/*
  ProcessLloydSites.cpp

  Li-Yi Wei
  08/14/2009

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

#include "Lloyd.hpp"
#include "DiscreteLloyd.hpp"

#include "LloydTools.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 5;
    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " input_file_name output_file_name dimension operation (fasten loosen increment decrement) class_ids (empty for all)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const string output_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const string operation = argv[++arg_ctr];

    vector<int> class_ids;
    while((arg_ctr+1) < argc)
    {
        class_ids.push_back(atoi(argv[++arg_ctr]));
    }

    // read input sites
    vector<Sample> samples;
    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    // process
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        Sample & sample = samples[i];

        int in_club = (class_ids.size() <= 0);
        for(unsigned int j = 0; !in_club && (j < class_ids.size()); j++)
        {
            if(sample.id == class_ids[j])
            {
                in_club = 1;
            }
        }

        if(in_club)
        {
            if(operation == "fasten")
            {
                LloydTools::FastenClassID(sample.id);
            }
            else if(operation == "loosen")
            {
                LloydTools::LoosenClassID(sample.id);
            }
            else if(operation == "increment")
            {
                sample.id++;
            }
            else if(operation == "decrement")
            {
                sample.id--;
            }
            else
            {
                // unrecognized operation
                // do nothing
                throw Exception("unrecognized operation");
            }
        }
    }

    // write out
    if(! Utility::WriteSamples(samples, output_file_name))
    {
        cerr << "cannot write to " << output_file_name << endl;
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
