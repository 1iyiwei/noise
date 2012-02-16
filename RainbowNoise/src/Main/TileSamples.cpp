/*
  TileSamples.cpp

  take a sample set and tile it into a larger one

  Li-Yi Wei
  10/08/2008

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "Sample.hpp"
#include "SequentialCounter.hpp"

int Main(int argc, char **argv)
{
    const int min_argc = 6;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input-file-name dimension scale num_tiles (dimension numbers) domain_size (dimension numbers)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const float scale = atof(argv[++argCtr]);

    if(argc < (min_argc + 2*(dimension-1)))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<int> num_tiles(dimension);
    for(unsigned int i = 0; i < num_tiles.size(); i++)
    {
        if((argCtr + 1) < argc)
        {
            num_tiles[i] = atoi(argv[++argCtr]);
        }
        else
        {
            throw Exception("not enough input arguments");
        }
    }
 
    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        if((argCtr + 1) < argc)
        {
            domain_size[i] = atof(argv[++argCtr]);
        }
        else
        {
            throw Exception("not enough input arguments");
        }
    }

    // get input samples
    ifstream input(input_file_name); // read in samples
    // cerr << "read in samples " << endl;
    
    vector<Sample> samples;
    
    if(!input)
    {
        return 0;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
            input >> sample.id;

            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    cerr << "total " << samples.size() << " samples read" << endl;

    // output
    SequentialCounter counter(dimension, vector<int>(dimension, 1), num_tiles);
    counter.Reset();

    vector<int> tile_index(dimension);
    do
    {
        counter.Get(tile_index);
        
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            cout << samples[i].id;
            
            for(int j = 0; j < samples[i].coordinate.Dimension(); j++)
            {
                cout << " " << (domain_size[j]*(tile_index[j]-1) + samples[i].coordinate[j])*scale;
            }

            cout << endl;
        }
    }
    while(counter.Next());

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
