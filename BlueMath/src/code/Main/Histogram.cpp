/*
  Histogram.cpp

  compute histogram for the sample coordinates

  Li-Yi Wei
  05/09/2008

  11/01/2008
  add allowance for multiple projection dimensions
  
  08/14/2010
  revert definition of _DIFF_HISTOGRAM
  remove freq option
  add sub_domain and max_diff options

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Math.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    const int min_argc = 7;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input-file-name-list dimension which_dimension(s) num_bins sub_domain (only consider diff inside subdomain) max_diff (maximum diff to consider, < 0 for default)" << endl;
        return 1;
    }

    int arg_ctr = 0;

    const string input_file_name_list = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);

    const int num_which_dimensions = argc - (min_argc + dimension - 1) + 1;
    if(num_which_dimensions <= 0)
    {
        cerr << "num_which_dimensions must be > 0" << endl;
        return 1;
    }

    vector<int> which_dimensions(num_which_dimensions);
    for(unsigned int i = 0; i < which_dimensions.size(); i++)
    {
        which_dimensions[i] = atoi(argv[++arg_ctr]);
        if( (which_dimensions[i] < 0) || (which_dimensions[i] >= dimension))
        {
            cerr << "illegal which_dimensions[" << i << "]" << endl;
            return 1;
        } 
    }

    const double frequency = 1; // atof(argv[++arg_ctr]);
    const int num_bins = atoi(argv[++arg_ctr]);

    vector<float> sub_domain(dimension);
    for(unsigned int i = 0; i < sub_domain.size(); i++)
    {
        sub_domain[i] = atof(argv[++arg_ctr]);
    }

    const float max_diff_input = atof(argv[++arg_ctr]);

    float max_diff_default = 0;
    for(unsigned int i = 0; i < sub_domain.size(); i++)
    {
        max_diff_default += sub_domain[i]*sub_domain[i];
    }
    max_diff_default = sqrt(max_diff_default);

    ifstream input_list(input_file_name_list.c_str());

    vector<string> input_file_names;

    if(!input_list)
    {
        cerr << "cannot read from " << input_file_name_list << endl;
        return 1;
    }   
    else
    {
        while(input_list.good())
        {
            string foo;
            input_list >> foo;
            if(input_list.good())
            {
                input_file_names.push_back(foo);
            }
        }
    }

    const float max_diff = max_diff_input >= 0 ? max_diff_input : max_diff_default;
    vector<double> bins(num_bins, 0);
    const float bin_size = max_diff/num_bins;
    double total_num_samples = 0;

    for(unsigned which_input = 0; which_input < input_file_names.size(); which_input++)
    {
        const string input_file_name = input_file_names[which_input];

        // get input samples
        vector<Sample> samples;
    
        if(! Utility::ReadSamples(dimension, input_file_name, samples))
        {
            cerr << "cannot read from " << input_file_name;
            return 1;
        }

        //cerr << "total " << samples.size() << " samples read" << endl;
    
        // accumulate
        for(unsigned int i = 0; i < samples.size(); i++)
#ifdef _DIFF_HISTOGRAM
        for(unsigned int j = i+1; j < samples.size(); j++)
#endif
        {
#ifdef _DIFF_HISTOGRAM
            const bool inside = Utility::Inside(sub_domain, samples[i]) || Utility::Inside(sub_domain, samples[j]);
#else
            const bool inside = Utility::Inside(sub_domain, samples[i]);
#endif

            if(!inside) continue;
 
            float diff = 0;
            
            for(unsigned int k = 0; k < which_dimensions.size(); k++)
            {
#ifndef _DIFF_HISTOGRAM
                const float dim_diff = samples[i].coordinate[which_dimensions[k]];
#else
                const float dim_diff = samples[i].coordinate[which_dimensions[k]] - samples[j].coordinate[which_dimensions[k]];
#endif           
                diff += dim_diff*dim_diff;
            }

            // diff = sqrt(diff/which_dimensions.size());
            diff = sqrt(diff);

            const float value = diff*frequency*1.0; // mod(diff*frequency*1.0, max_diff*1.0); // notice the alignment by difference

            const int which_bin = static_cast<int>(floor(value/bin_size));
            
            if((which_bin < 0) || (which_bin >= num_bins))
            {
                // out of bound
                // throw Exception("weird which_bin");
            }
            else
            {
                bins[which_bin]++;
                total_num_samples++;
            }
        }
    }

    // cerr << "total_num_samples " << total_num_samples << endl; // debug

    // output in gnuplot data style
    for(unsigned int i = 0; i < bins.size(); i++)
    {
        cout << i*bin_size << " " << bins[i]*1.0*num_bins/total_num_samples << endl;
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
