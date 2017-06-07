/*
  SelectSamples

  randomly select a subset from an input sample set

  Li-Yi Wei
  11/24/2007

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Random.hpp"
#include "Exception.hpp"
#include "SFT.hpp"

int Main(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " input-file-name output-file-name dimension num-output-samples" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const int num_output_samples = atoi(argv[++argCtr]);

    if(num_output_samples < 0)
    {
        cerr << "num_output_samples must be >= 0" << endl;
        return 1;
    }
    
    ifstream input(input_file_name); // read in samples
    cout << "read in samples " << endl;
    
    vector<Sample> samples;

    // read samples
    if(!input)
    {
        cerr << "cannot open " << input_file_name << " for read" << endl;
        return 1;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
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

    cout << "total " << samples.size() << " samples read" << endl;

    // select samples
    Random::InitRandomNumberGenerator();

    while(samples.size() > num_output_samples)
    {
        int selection = static_cast<int>(floor(Random::UniformRandom() * samples.size()));

        if(selection >= samples.size()) selection = samples.size()-1;
        samples.erase(samples.begin()+selection);
    }

    if(samples.size() != num_output_samples)
    {
        cerr << "samples.size() != num_output_samples" << endl;
        return 1;
    }
    
    // output samples
    ofstream output(output_file_name);
    if(! output)
    {
        cerr << "cannot open " << output_file_name << " for write" << endl;
        return 1;
    }
    else
    {
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            for(int j = 0; j < samples[i].coordinate.Dimension(); j++)
            {
                output << samples[i].coordinate[j] << " ";
            }
            output << endl;
        }
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
