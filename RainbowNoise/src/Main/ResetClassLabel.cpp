/*
  ResetClassLabel.cpp

  reset the class id/label for a sample set
  in preparation for DiscreteDartThrowing

  Li-Yi Wei
  10/17/2008

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>

#include "Sample.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    int min_argc = 3;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension input_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const char * input_file_name = argv[++argCtr];
    const int reset_class_id = -1;
    
    // input
    ifstream input(input_file_name);
    vector<Sample> input_samples;

    if(! input)
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
                input_samples.push_back(sample);
            }
        }
    }

    // output
    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        cout << reset_class_id;
        for(int j = 0; j < input_samples[i].coordinate.Dimension(); j++)
        {
            cout << " " << input_samples[i].coordinate[j];
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
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
