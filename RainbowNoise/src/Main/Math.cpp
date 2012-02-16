/*
  Math.cpp

  a collection of math verification/computing stuff 

  Li-Yi Wei
  05/08/2008

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Math.hpp"
#include "Exception.hpp"
#include "Random.hpp"
#include "Sample.hpp"

int SFT1DUniformRandom(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " frequency (integer) num_samples num_runs" << endl;
        return 1;
    }

    int argCtr = 0;
    const int frequency = atoi(argv[++argCtr]);
    const int num_samples = atoi(argv[++argCtr]);
    const int num_trials = atoi(argv[++argCtr]);
    const double PI = 3.14159265;

    Random::InitRandomNumberGenerator();

    double final = 0;
    for(int trial = 0; trial < num_trials; trial++)
    {
        double mean_cos = 0;
        double mean_sin = 0;
        for(int i = 0; i < num_samples; i++)
        {
            const double value = Random::UniformRandom();
            mean_cos += cos(2*PI*frequency*value);
            mean_sin += sin(2*PI*frequency*value);
        }

        mean_cos /= num_samples;
        mean_sin /= num_samples;

        final += mean_cos*mean_cos + mean_sin*mean_sin*0;
    }

    cout << "final: " << final/num_trials*num_samples << endl;

    return 0;
}

int MeanCosUniformRandom(int argc, char **argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " num_samples" << endl;
        return 1;
    }

    int argCtr = 0;
    const int num_samples = atoi(argv[++argCtr]);
    const double PI = 3.14159265;

    Random::InitRandomNumberGenerator();

    double final = 0;

    double mean_cos = 0;
    double mean_sin = 0;
    for(int i = 0; i < num_samples; i++)
    {
        const double value = Random::UniformRandom();
        mean_cos += cos(2*PI*value); // E(X)
        //mean_cos += pow(cos(2*PI*value), 2.0);
        //mean_cos += pow(pow(cos(2*PI*value), 2.0) - 0.5, 2.0);

        mean_sin += sin(2*PI*value); 
    }

    mean_cos /= num_samples;
    mean_sin /= num_samples;
    
    cout << "mean_cos: " << mean_cos << endl;

    //cout << "final: " << (mean_cos*mean_cos + mean_sin*mean_sin)*num_samples << endl;

    return 0;
}

struct Complex
{
    Complex(void) : r(0), i(0) {};

    double r;
    double i;
};

int SFT1DSamples(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " input_list dimension which_dimension max_frequency" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name_list = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const int which_dimension = atoi(argv[++argCtr]);
    const int max_frequency = atoi(argv[++argCtr]);
    const double PI = 3.14159265;

    if((which_dimension < 0) || (which_dimension >= dimension))
    {
        cerr << "illegal which_dimension" << endl;
        return 1;
    }   

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

    vector< vector<Complex> > sft_values_all(input_file_names.size(), vector<Complex>(max_frequency));

    double total_num_samples = 0;
    for(unsigned int trial = 0; trial < input_file_names.size(); trial++)
    {
        const string input_file_name = input_file_names[trial];

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

        for(unsigned int frequency = 0; frequency < sft_values_all[trial].size(); frequency++)
        {
            double mean_cos = 0;
            double mean_sin = 0;
            for(unsigned int i = 0; i < samples.size(); i++)
            {
                const double value = samples[i].coordinate[which_dimension];
                mean_cos += cos(2*PI*frequency*value);
                mean_sin += sin(2*PI*frequency*value);
            }

            mean_cos /= samples.size();
            mean_sin /= samples.size();

            //sft_values[frequency] += mean_cos; // *mean_cos + mean_sin*mean_sin;
            sft_values_all[trial][frequency].r = mean_cos;
            sft_values_all[trial][frequency].i = mean_sin;
        }
            
        total_num_samples += samples.size();
    }

    // summary
    vector<Complex> sft_means(max_frequency);
    for(unsigned int frequency = 0; frequency < sft_means.size(); frequency++)
    {
        for(unsigned int trial = 0; trial < sft_values_all.size(); trial++)
        {
            sft_means[frequency].r += sft_values_all[trial][frequency].r;
            sft_means[frequency].i += sft_values_all[trial][frequency].i;
        }
        
        sft_means[frequency].r /= sft_values_all.size();
        sft_means[frequency].i /= sft_values_all.size();
    }

    vector<double> sft_values(max_frequency, 0);
    for(unsigned int frequency = 0; frequency < sft_values.size(); frequency++)
    {
        for(unsigned int trial = 0; trial < sft_values_all.size(); trial++)
        {
            const double sft_cos = sft_values_all[trial][frequency].r - sft_means[frequency].r;
            const double sft_sin = sft_values_all[trial][frequency].i - sft_means[frequency].i;

            sft_values[frequency] += sft_cos*sft_cos + sft_sin*sft_sin;
        }

        sft_values[frequency] /= sft_values_all.size();
    }

    // output
    for(unsigned int frequency = 1; frequency < sft_values.size(); frequency++)
    {
        cout << frequency << " " << sft_values[frequency] * (total_num_samples/input_file_names.size()) << endl;;
    }

    return 0;
}

int Rho(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " dimension r_input num_samples" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float r_input = atof(argv[++argCtr]);
    const float num_samples = atof(argv[++argCtr]);

    // cout << "MaxMinDistance(" << dimension << ", " << num_samples << ") == " << Math::ComputeMaxMinDistance(dimension, num_samples) << endl;
    cout << r_input/Math::ComputeMaxMinDistance(dimension, num_samples);

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

        // return SFT1DUniformRandom(argc, argv);
        // return MeanCosUniformRandom(argc, argv);
        if(option == "sft1dsamples")
        {
            return SFT1DSamples(argc-1, &argv[1]);
        }
        else if(option == "rho")
        {
            return Rho(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown math option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
