/*
  Test.cpp

  a collection of routines for testing stuff

  Li-Yi Wei
  08/21/2008

*/


#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Math.hpp"
#include "Exception.hpp"
#include "Random.hpp"

#include "SFT.hpp"
#include "LFT.hpp"
#include "LinearLFT.hpp"

#include "SequentialCounter.hpp"

int LFTWhiteNoise(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " dimension num_samples max_freq" << endl;
        return 1;
    }
        
    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int num_samples = atoi(argv[++argCtr]);
    const int max_freq = atoi(argv[++argCtr]);

    // init random number
    Random::InitRandomNumberGenerator(time(0));

    // generate samples
    vector<Sample> samples;

    Sample sample(dimension);
    
    sample.value = 1.0/num_samples;

    for(int i = 0; i < num_samples; i++)
    {
        for(int j = 0; j < sample.coordinate.Dimension(); j++)
        {
            sample.coordinate[j] = Random::UniformRandom();
        }

        samples.push_back(sample);
    }

    // evaluators
    SFT sft(samples);
    // LFT lft(samples);
    LinearLFT lft(samples);

    // see if they are equal
    float max_diff = -1;
    float max_value = -1;

    SequentialCounter counter(dimension, 0, max_freq);
    vector<int> count(dimension);
    Coordinate w(dimension);

    counter.Reset();
    do
    {
        counter.Get(count);

        for(int i = 0; i < w.Dimension(); i++)
        {
            w[i] = 2*Math::PI*count[i];
        }

        const float sft_value = sft.Evaluate(w);
        const float lft_value = lft.Evaluate(w);
        const float diff = fabs(sft_value - lft_value);

        if(diff > max_diff) max_diff = diff;

        // if(sft_value > max_value) max_value = sft_value;
        if(lft_value > max_value) max_value = lft_value;
    }
    while(counter.Next());

    cout << "max diff: " << max_diff << ", max value: " << max_value << endl;

    // done 
    return 0;
}

#include "Sample.hpp"
#include "DeformableLFT.hpp"
#include "Math.hpp"

void Randomize(const vector<float> & domain_size, Coordinate & s)
{
    for(int i = 0; i < s.Dimension(); i++)
    {
        s[i] = Random::UniformRandom()*domain_size[i];
    }
}

void Shift(const vector<float> & domain_size, const vector<float> & shift_amount, const Coordinate & input, Coordinate & output)
{
    for(int i = 0; i < output.Dimension(); i++)
    {
        output[i] = mod(input[i] + shift_amount[i], domain_size[i]);
    }
}

int ShiftInvariance(int argc, char **argv)
{
    int min_argc = 4;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension num_trials domain_size tolerance" << endl;
        return 1;
    }
        
    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int num_trials = atoi(argv[++argCtr]);
    if(argc < (min_argc + dimension))
    {
        cerr << "insufficient number of arguments" << endl;
        return 1;
    }
    
    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++argCtr]);
    }

    const float tolerance = atof(argv[++argCtr]);

    // init random number
    Random::InitRandomNumberGenerator(time(0));

    // do trials
    Coordinate s1(dimension);
    Coordinate s2(dimension);
    Coordinate s1_shifted(dimension);
    Coordinate s2_shifted(dimension);
    vector<float> shift_amount(dimension);

    for(int which_trial = 0; which_trial < num_trials; which_trial++)
    {
        // get samples
        Randomize(domain_size, s1); Randomize(domain_size, s2);

        // randomly shift them together, but do so toroidally with domain_size
        for(unsigned int i = 0; i < shift_amount.size(); i++)
        {
            shift_amount[i] = (Random::UniformRandom()*2-1)*domain_size[i];
        }

        Shift(domain_size, shift_amount, s1, s1_shifted);
        Shift(domain_size, shift_amount, s2, s2_shifted);

        // check to see if their distances are consistent
        for(int i = 0; i < dimension; i++)
        {
            if(fabs(DeformableLFT::Diff(domain_size[i], s1[i], s2[i]) - DeformableLFT::Diff(domain_size[i], s1_shifted[i], s2_shifted[i])) > tolerance)
            {
                cout << DeformableLFT::Diff(domain_size[i], s1[i], s2[i]) << " = Diff(" << domain_size[i] << ", " << s1[i] << ", " << s2[i] << ") != " << DeformableLFT::Diff(domain_size[i], s1_shifted[i], s2_shifted[i]) << " = Diff(" << domain_size[i] << ", " << s1_shifted[i] << ", " << s2_shifted[i] << ")" << endl;
                return 1;
            }
        }
    }

    // done
    cout << "all the same" << endl;
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

        if(option == "lft")
        {
            return LFTWhiteNoise(argc-1, &argv[1]);
        }
        else if(option == "shift_invariance")
        {
            return ShiftInvariance(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown test option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
