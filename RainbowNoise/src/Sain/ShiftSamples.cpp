/*
  ShiftSamples.cpp

  shift a sample set toroidally to faciliate distance banding
  (overally this does not affect power spectrum)

  Li-Yi Wei
  08/30/2008

*/

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <float.h>
//#include <limits.h>

#include "Sample.hpp"
#include "Exception.hpp"
#include "Math.hpp"

int Main(int argc, char **argv)
{
    int min_arg = 4;

    if(argc < min_arg)
    {
        cerr << "Usage: " << argv[0] << " input-file-name output-file-name dimension domain_size (dimension numbers) shift_amount (dimension numbers)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);

    if(dimension <= 0)
    {
        cerr << "dimension <= 0" << endl;
        return 1;
    }

    if(argc < (min_arg + 2*dimension))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension);
    vector<float> shift_amount(dimension);

    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++argCtr]);
    }
    for(unsigned int i = 0; i < shift_amount.size(); i++)
    {
        shift_amount[i] = atof(argv[++argCtr]);
    }

    // read in samples
    cerr << "read in samples " << endl;
    
    vector<Sample> samples;
    
    ifstream input(input_file_name);
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

    // process
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        for(int j = 0; j < dimension; j++)
        {
            samples[i].coordinate[j] = mod(samples[i].coordinate[j] + shift_amount[j], domain_size[j]);
        }
    }

    // output
    ofstream output(output_file_name);

    if(!output)
    {
        cerr << "cannot write to " << output_file_name << endl;
    }
    else
    {
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            for(int j = 0; j < dimension; j++)
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
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
