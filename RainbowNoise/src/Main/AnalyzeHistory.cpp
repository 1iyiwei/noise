/*
  AnalyzeHistory.cpp

  analyze the history records dumped out by DartThrowingHistory

  Li-Yi Wei
  06/24/2008

*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "SampleRecord.hpp"
#include "Math.hpp"

int CDF(int argc, char **argv)
{    
    const int num_fixed_arguments = 6;
    if(argc < (num_fixed_arguments + 2))
    {
        cerr << "Usage: " << argv[0] << " input_file_name dimension num_classes r_values (c numbers) normalization_option (0 for no-normalization, [0 1] for rho, 1 for normalization to final number) max_num_time_samples output_file_names (c numbers)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const int num_classes = atoi(argv[++argCtr]);
    if(num_classes < 1)
    {
        cerr << "num_classes must be >= 1" << endl;
        return 1;
    }

    if(argc < (num_fixed_arguments + num_classes*2))
    {
        cerr << "insufficient number of input arguments" << endl;
        return 1;
    }

    vector<float> r_values(num_classes);
    for(unsigned int i = 0; i < r_values.size(); i++)
    {
        r_values[i] = atof(argv[++argCtr]);
    }

    const float normalization_option = atof(argv[++argCtr]);
    const int max_num_time_samples = atoi(argv[++argCtr]);

    vector<const char *> output_file_names(num_classes);

    for(unsigned int i = 0; i < output_file_names.size(); i++)
    {
        output_file_names[i] = argv[++argCtr];
    }

    // input
    vector<SampleRecord> sample_history;

    if(! ReadSampleHistory(input_file_name, sample_history))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    // analysis
    vector< vector<int> > output_data(num_classes, vector<int>(sample_history.size(), 0));

    for(unsigned int i = 0; i < sample_history.size(); i++)
    {
        if(i > 0)
        {
            for(unsigned int j = 0; j < output_data.size(); j++)
            {
                output_data[j][i] += output_data[j][i-1];
            }
        }

        const SampleRecord current = sample_history[i];

        if((current.class_id < 0) || (current.class_id >= num_classes))
        {
            cerr << "sample class id out of bound" << endl;
            return 1;
        }

        if(current.status == SampleRecord::ACCEPTED)
        {
            output_data[current.class_id][i] += 1;
        }

        if(current.status == SampleRecord::KILLED)
        {
            output_data[current.class_id][i] -= 1;
        }
    }

    // output
    for(unsigned int which_output = 0; which_output < output_data.size(); which_output++)
    {
        const char * output_file_name = output_file_names[which_output];

        ofstream output(output_file_name);

        if(!output)
        {
            cerr << "cannot write to " << output_file_name << endl;
            return 1;
        }
        else
        {
            const int step = static_cast<int>(ceil(output_data[which_output].size()*1.0/max_num_time_samples));

            float max_num_samples = 0;

            if((normalization_option > 0) && (normalization_option < 1))
            {
                const float rho_number = normalization_option;
                max_num_samples = Math::ComputeMaxNumSamples(dimension, r_values[which_output]/rho_number);
            }
            else if(normalization_option == 1)
            {
                max_num_samples = output_data[which_output].size()-1;
            }
            else if(normalization_option == 0)
            {
                // do nothing
            }
            else
            {
                cerr << "unknown normalization option" << endl;
                return 1;
            }

            for(unsigned int i = 0; i < output_data[which_output].size(); i += step)
            {
                if(normalization_option > 0)
                {
                    output << i*1.0/(output_data[which_output].size()-1) << " " << output_data[which_output][i]*1.0/max_num_samples;
                }
                else
                {
                    output << i << " " << output_data[which_output][i];
                }

                output << endl;
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
        if(argc < 2)
        {
            cerr << "Usage: " << argv[0] << " option" << endl;
            return 1;
        }

        int argCtr = 0;
        const string option = argv[++argCtr];

        if(option == "cdf")
        {
            return CDF(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown histogram-analysis option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
