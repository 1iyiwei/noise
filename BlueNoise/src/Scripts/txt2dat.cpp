/*
  convert from line txt to dat format for gnuplot

  Li-Yi Wei
  07/06/2007

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
            output << i+1 << " " << input[i] << endl;
        }
        
        return 1;
    }
}

int main(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " input-mean input_variance [reference-mean reference-variance] output-mean output-variance" << endl;
        return 1;
    }

    const int has_reference = (argc >= 7);
    
    int argCtr = 0;
    const char * input_mean_file_name = argv[++argCtr];
    const char * input_variance_file_name = argv[++argCtr];
    const char * reference_mean_file_name = has_reference ? argv[++argCtr] : 0;
    const char * reference_variance_file_name = has_reference ? argv[++argCtr] : 0;
    const char * output_mean_file_name = argv[++argCtr];
    const char * output_variance_file_name = argv[++argCtr];

    // input
    deque<float> input_mean, input_variance;
    deque<float> reference_mean, reference_variance;
    
    if(! Read(input_mean_file_name, input_mean))
    {
        cerr << "error in reading " << input_mean_file_name << endl;
        return 1;
    }
    if(! Read(input_variance_file_name, input_variance))
    {
        cerr << "error in reading " << input_variance_file_name << endl;
        return 1;
    }

    if(has_reference)
    {
        if(! Read(reference_mean_file_name, reference_mean))
        {
            cerr << "error in reading " << reference_mean_file_name << endl;
            return 1;
        }
        if(! Read(reference_variance_file_name, reference_variance))
        {
            cerr << "error in reading " << reference_variance_file_name << endl;
            return 1;
        }
    }
    
    // compute
    float reference_mean_average = 0;
    {
        for(unsigned int i = 0; i < reference_mean.size(); i++)
        {
            reference_mean_average += reference_mean[i];
        }

        if(reference_mean.size())
        {
            reference_mean_average /= reference_mean.size();
        }
    }

    if(reference_mean_average > 0)
    {
        for(unsigned int i = 0; i < input_mean.size(); i++)
        {
            input_mean[i] /= reference_mean_average;
        }
    }
    else
    {
        for(unsigned int i = 0; i < input_mean.size(); i++)
        {
            input_mean[i] /= input_mean[input_mean.size()-1];
        }
    }
    
    // output
    if(! Write(output_mean_file_name, input_mean))
    {
        cerr << "error in writing " << output_mean_file_name << endl;
        return 1;
    }
    if(! Write(output_variance_file_name, input_variance))
    {
        cerr << "error in writing " << output_variance_file_name << endl;
        return 1;
    }

    // done
    return 0;
}
