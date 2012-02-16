/*
  MergeSamples.cpp

  merge (multi-class) samples from multiple sets into one

  Li-Yi Wei
  07/04/2008

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
    const int num_fixed_arguments = 2;
    if(argc < num_fixed_arguments)
    {
        cerr << "Usage: " << argv[0] << " input-list-file-name dimension" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_list_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);

    vector<string> input_file_names;
    {
        ifstream input_file_list(input_list_file_name);
        if(!input_file_list)
        {
            cerr << "cannot read from " << input_list_file_name << endl;
            return 1;
        }
        else
        {
            while(input_file_list.good())
            {
                string input_file_name;
                input_file_list >> input_file_name;

                if(input_file_list.good())
                {
                    input_file_names.push_back(input_file_name);
                }
            }
        }
    }
     
    int last_set_id = -1;

    for(unsigned int which_input = 0; which_input < input_file_names.size(); which_input++)
    {
        string input_file_name = input_file_names[which_input];

        // get input samples
        ifstream input(input_file_name.c_str()); // read in samples
        // cerr << "read in samples " << endl;
    
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
        
        // cerr << "total " << samples.size() << " samples read" << endl;
    
        // output
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            // output
            cout << (samples[i].id + last_set_id + 1) << " ";
            for(int k = 0; k < samples[i].coordinate.Dimension(); k++)
            {
                cout << samples[i].coordinate[k] << " ";
            }
            cout << endl;
        }

        // update last_set_id
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            if(samples[i].id > last_set_id)
            {
                last_set_id = samples[i].id;
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
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
