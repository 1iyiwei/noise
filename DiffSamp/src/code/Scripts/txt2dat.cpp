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

int Write(const char * file_name, const deque<float> & input, const float freq_scale)
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
            output << (i+1)*freq_scale << " " << input[i] << endl;
        }
        
        return 1;
    }
}

int main(int argc, char **argv)
{
    if(argc < 7)
    {
        cerr << "Usage: " << argv[0] << " input-mean input_variance num_samples output-mean output-variance freq_scale (1 for ordinary situations)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_mean_file_name = argv[++argCtr];
    const char * input_variance_file_name = argv[++argCtr];
    const float num_samples = atof(argv[++argCtr]);
    const char * output_mean_file_name = argv[++argCtr];
    const char * output_variance_file_name = argv[++argCtr];
    const float freq_scale = atof(argv[++argCtr]);

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

    // compute
    const float reference_mean_average = 1.0/num_samples;

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
    if(! Write(output_mean_file_name, input_mean, freq_scale))
    {
        cerr << "error in writing " << output_mean_file_name << endl;
        return 1;
    }
    if(! Write(output_variance_file_name, input_variance, freq_scale))
    {
        cerr << "error in writing " << output_variance_file_name << endl;
        return 1;
    }

    // done
    return 0;
}
