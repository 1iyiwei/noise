/*
  RandomizeSamples.cpp

  Given a set of samples, randomize their IDs 
  (but keep the relative counts the same)

  Li-Yi Wei
  05/15/2008

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Exception.hpp"
#include "Sample.hpp"
#include "Random.hpp"

int Main(int argc, char **argv)
{
    const int num_fixed_arguments = 3;
    if(argc < num_fixed_arguments)
    {
        cerr << "Usage: " << argv[0] << " dimension input-file-name" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const char * input_file_name = argv[++argCtr];

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

    // collect the ids
    vector<int> sample_ids(samples.size());
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        sample_ids[i] = samples[i].id;
    }

    // and randomize that
    Random::InitRandomNumberGenerator(time(0));
    {
        vector<int> randomized_sample_ids;

        while(sample_ids.size() > 0)
        {
            const int selection = static_cast<int>(floor(Random::UniformRandom()*sample_ids.size()))%sample_ids.size();
            randomized_sample_ids.push_back(sample_ids[selection]);
            sample_ids[selection] = sample_ids[sample_ids.size()-1];
            sample_ids.pop_back();
        }

        sample_ids = randomized_sample_ids;
    }

    // output
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        cout << sample_ids[i] << " ";
        for(int k = 0; k < samples[i].coordinate.Dimension(); k++)
        {
            cout << samples[i].coordinate[k] << " ";
        }
        cout << endl;
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
