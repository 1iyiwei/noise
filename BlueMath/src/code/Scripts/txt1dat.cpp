/*
  txt1dat.cpp

  add index information to a 1D data sequence for gnuplot
  named so since txt2dat converts 2 stuff: mean and variance
  while this program converts only 1

  Li-Yi Wei
  07/04/2009

*/

#include <iostream>
#include <fstream>
#include <deque>
using namespace std;

#include <stdlib.h>
#include <math.h>

int Read(const char * file_name, deque<float> & result)
{
    ifstream input(file_name);

    if(! input)
    {
        return 0;
    }
    else
    {
        result.clear();
        
        float data = 0;
        while(input.good())
        {
            input >> data;

            if(input.good())
            {
                result.push_back(data);
            }
        }

        return 1;
    }
}

int Write(const char * file_name, const deque<float> & input)
{
    ofstream output(file_name);

    if(! output)
    {
        return 0;
    }
    else
    {
        for(unsigned int i = 0; i < input.size(); i++)
        {
            output << (i+1) << " " << input[i] << endl;
        }
        
        return 1;
    }
}

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " input_file_name output_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];

    // input
    deque<float> input_sequence;
    
    if(! Read(input_file_name, input_sequence))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }

    // output
    if(! Write(output_file_name, input_sequence))
    {
        cerr << "error in writing " << output_file_name << endl;
        return 1;
    }

    // done
    return 0;
}
