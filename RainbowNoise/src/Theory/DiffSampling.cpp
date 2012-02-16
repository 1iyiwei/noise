/*
  DiffSampling.cpp

  sample diff pairs from an existing set of samples

  Li-Yi Wei
  11/05/2008

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Sample.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    const int min_argc = 4;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_file_name (one class without class id) dimension num_pairs" << endl;

        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const int num_pairs = atoi(argv[++argCtr]);

    // read samples    
    ifstream input(input_file_name);

    vector<Sample> samples;
    
    if(!input)
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
            sample.id = 0;

#ifdef _SFT_READ_VALUE
            input >> sample.value;
#endif
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

    const int num_samples = samples.size();
    
    const int max_num_pairs = num_samples*(num_samples - 1)/2;

    // pre-process
    vector< vector<unsigned int> > pair_indices;
    vector<unsigned int> pair(2);
    for(unsigned int i = 0; i < samples.size(); i++)
        for(unsigned int j = i+1; j < samples.size(); j++)
        {
            pair[0] = i; pair[1] = j;

            pair_indices.push_back(pair);
        }

    if(num_pairs > pair_indices.size())
    {
        cerr << "num_pairs > max_num_pairs" << endl;
        return 1;
    }

    // run
    Random::InitRandomNumberGenerator();
    
    for(int which_pair = 0; which_pair < num_pairs; which_pair++)
    {
        const int select = static_cast<int>(floor(pair_indices.size()*Random::UniformRandom()))%pair_indices.size();

        for(int k = 0; k < dimension; k++)
        {
            cout << samples[pair_indices[select][0]].coordinate[k] - samples[pair_indices[select][1]].coordinate[k] << " ";
        }
        cout << endl;

        pair_indices[select] = pair_indices[pair_indices.size()-1];
        pair_indices.pop_back();
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
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
