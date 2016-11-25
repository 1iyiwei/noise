/*
  behead.cpp

  chop off the leading portions of a .dat file
  e.g. unreliable anisotropy due to lack of low frequency data

  Li-Yi Wei
  10/14/2010

*/

#include <iostream>
#include <fstream>
#include <string>
#include <deque>
using namespace std;

#include <stdlib.h>
#include <math.h>

struct Datum
{
    Datum(void): index(0), value(0) {};

    float index;
    float value;
};

int Read(const string & file_name, deque<Datum> & result)
{
    ifstream input(file_name.c_str());

    if(! input)
    {
        return 0;
    }
    else
    {
        result.clear();
        
        Datum data;
        while(input.good())
        {
            input >> data.index >> data.value;

            if(input.good())
            {
                result.push_back(data);
            }
        }

        return 1;
    }
}

int Write(const string & file_name, const deque<Datum> & input)
{
    ofstream output(file_name.c_str());

    if(! output)
    {
        return 0;
    }
    else
    {
        for(unsigned int i = 0; i < input.size(); i++)
        {
            output << input[i].index << " " << input[i].value << endl;
        }
        
        return 1;
    }
}

int main(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " input output index_threshold" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const string output_file_name = argv[++arg_ctr];
    const float index_threshold = atof(argv[++arg_ctr]);

    // input
    deque<Datum> input;
    
    if(! Read(input_file_name, input))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }

    // compute
    deque<Datum> output;

    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(input[i].index >= index_threshold)
        {
            output.push_back(input[i]);
        }
    }

    // output
    if(! Write(output_file_name, output))
    {
        cerr << "error in writing " << output_file_name << endl;
        return 1;
    }

    // done
    return 0;
}
