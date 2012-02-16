/*
  ProjectSamples.cpp

  project samples to a lower dimensional space

  Li-Yi Wei
  05/09/2008

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "Sample.hpp"

int Main(int argc, char **argv)
{
    const int num_fixed_arguments = 3;
    if(argc < num_fixed_arguments)
    {
        cerr << "Usage: " << argv[0] << " input-file-name dimension dimension-id (multiple)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);

    vector<int> dimension_ids;
    while((argCtr + 1) < argc)
    {
        dimension_ids.push_back(atoi(argv[++argCtr]));
    }

    for(unsigned int i = 0; i < dimension_ids.size(); i++)
    {
        if((dimension_ids[i] < 0) || (dimension_ids[i] >= dimension))
        {
            cerr << "out of range dimension_id" << endl;
            return 1;
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
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        cout << samples[i].id;

        for(unsigned int j = 0; j < dimension_ids.size(); j++)
        {
            cout << " " << samples[i].coordinate[dimension_ids[j]];
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
