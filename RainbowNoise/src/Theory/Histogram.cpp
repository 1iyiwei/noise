/*
  Histogram.cpp

  compute histograms for the sample coordinates

  Li-Yi Wei
  05/09/2008
  11/01/2008 add allowance for multiple projection dimensions

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "Sample.hpp"

template<class T>
T mod(const T a, const T b)
{
    return (a-floor(a/b)*b);
}

int Main(int argc, char **argv)
{
    const int min_argc = 6;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input-file-name-list dimension which_dimension(s) frequency num_bins" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name_list = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const int num_which_dimensions = argc - min_argc + 1;
    if(num_which_dimensions <= 0)
    {
        cerr << "num_which_dimensions must be > 0" << endl;
        return 1;
    }
    vector<int> which_dimensions(num_which_dimensions);
    for(unsigned int i = 0; i < which_dimensions.size(); i++)
    {
        which_dimensions[i] = atoi(argv[++argCtr]);
        if( (which_dimensions[i] < 0) || (which_dimensions[i] >= dimension))
        {
            cerr << "illegal which_dimensions[" << i << "]" << endl;
            return 1;
        } 
    }
    const double frequency = atof(argv[++argCtr]);
    const int num_bins = atoi(argv[++argCtr]);
    const double domain_size = 1.0;

    ifstream input_list(input_file_name_list);

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

    const float max_diff = domain_size*sqrt(1.0*which_dimensions.size());
    vector<double> bins(num_bins, 0);
    const float bin_size = max_diff/num_bins;
    double total_num_samples = 0;

    for(unsigned which_input = 0; which_input < input_file_names.size(); which_input++)
    {
        const string input_file_name = input_file_names[which_input];

        // get input samples
        ifstream input(input_file_name.c_str()); // read in samples
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

        //cerr << "total " << samples.size() << " samples read" << endl;
    
        // accumulate
        for(unsigned int i = 0; i < samples.size(); i++)
#ifndef _DIFF_HISTOGRAM
        for(unsigned int j = i+1; j < samples.size(); j++)
#endif
        {
            float diff = 0;
            
            for(unsigned int k = 0; k < which_dimensions.size(); k++)
            {
#ifdef _DIFF_HISTOGRAM
                const float dim_diff = samples[i].coordinate[which_dimensions[k]];
#else
                const float dim_diff = samples[i].coordinate[which_dimensions[k]] - samples[j].coordinate[which_dimensions[k]];
#endif           
                diff += dim_diff*dim_diff;
            }

            // diff = sqrt(diff/which_dimensions.size());
            diff = sqrt(diff);

            const float value = mod(diff*frequency*1.0, max_diff*1.0); // notice the alignment by difference

            const int which_bin = static_cast<int>(floor(value/bin_size))%num_bins;
            
            if((which_bin < 0) || (which_bin >= num_bins))
            {
                throw Exception("weird which_bin");
            }

            bins[which_bin]++;
            total_num_samples++;
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
