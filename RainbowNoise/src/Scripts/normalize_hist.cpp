/*
  normalize one histogram with respect to another by division

  Li-Yi Wei
  11/10/2008

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
            output << input[i] << " ";
        
            if(i%2) output << endl;
        }

        return 1;
    }
}

int main(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " input reference output" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * reference_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];

    // input
    deque<float> input, reference;
    
    if(! Read(input_file_name, input))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }
    if(! Read(reference_file_name, reference))
    {
        cerr << "error in reading " << reference_file_name << endl;
        return 1;
    }

    if(input.size() != reference.size())
    {
        cerr << "input.size() != reference.size()" << endl;
        return 1;
    }

    // compute
    for(unsigned int i = 1; i < input.size(); i+=2)
    {
        if(reference[i] > 0) input[i] /= reference[i];
    }

    // output
    if(! Write(output_file_name, input))
    {
        cerr << "error in writing " << output_file_name << endl;
        return 1;
    }

    // done
    return 0;
}
