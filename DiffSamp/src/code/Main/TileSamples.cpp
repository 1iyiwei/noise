/*
  TileSamples.cpp

  take a sample set and tile it into a larger one

  Li-Yi Wei
  10/08/2008

  11/02/2010
  input mode

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Utility.hpp"
#include "Exception.hpp"
#include "SequentialCounter.hpp"

int Main(int argc, char **argv)
{
    const int min_argc = 6;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input-file-name dimension scale num_tiles (dimension numbers) domain_size (dimension numbers)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const float scale = atof(argv[++arg_ctr]);

    if(argc < (min_argc + 2*(dimension-1)))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<int> num_tiles(dimension);
    for(unsigned int i = 0; i < num_tiles.size(); i++)
    {
        if((arg_ctr + 1) < argc)
        {
            num_tiles[i] = atoi(argv[++arg_ctr]);
        }
        else
        {
            throw Exception("not enough input arguments");
        }
    }
 
    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        if((arg_ctr + 1) < argc)
        {
            domain_size[i] = atof(argv[++arg_ctr]);
        }
        else
        {
            throw Exception("not enough input arguments");
        }
    }

    // get input samples
    vector<Sample> input_samples;
    
    if(! Utility::ReadSamples(dimension, input_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_file_name << endl;
    }

    cerr << "total " << input_samples.size() << " samples read" << endl;

    // process
    SequentialCounter counter(dimension, vector<int>(dimension, 1), num_tiles);
    counter.Reset();

    vector<Sample> output_samples;
    Sample sample;

    vector<int> tile_index(dimension);
    do
    {
        counter.Get(tile_index);
        
        for(unsigned int i = 0; i < input_samples.size(); i++)
        {
            sample = input_samples[i];
            
            for(int j = 0; j < sample.coordinate.Dimension(); j++)
            {
                sample.coordinate[j] = (domain_size[j]*(tile_index[j]-1) + input_samples[i].coordinate[j])*scale;
            }

            output_samples.push_back(sample);
        }
    }
    while(counter.Next());

    // output
    string output_file_name;

    if((arg_ctr + 1) < argc)
    {
        output_file_name = argv[++arg_ctr];
    }
    else
    {
        output_file_name = "cout";
    }

    if(! Utility::WriteSamples(output_samples, output_file_name))
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
