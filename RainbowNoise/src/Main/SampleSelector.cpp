/*
  SampleSelector.cpp

  select samples with specific ids

  Li-Yi Wei
  04/27/2008

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
        cerr << "Usage: " << argv[0] << " input-file-name dimension group-ids (multiple)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);

    vector<int> ids;
    while((argCtr + 1) < argc)
    {
        ids.push_back(atoi(argv[++argCtr]));
    }

    // get input samples
    ifstream input(input_file_name); // read in samples
    // cerr << "read in samples " << endl;
    
    vector<Sample> samples;
    
    if(!input)
    {
        cerr << "cannot read from " << input_file_name;
        return 1;
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
        for(unsigned int j = 0; j < ids.size(); j++)
        {
            if(samples[i].id == ids[j])
            {
                // output

                for(int k = 0; k < samples[i].coordinate.Dimension(); k++)
                {
                    cout << samples[i].coordinate[k] << " ";
                }
                cout << endl;

                break;
            }
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
