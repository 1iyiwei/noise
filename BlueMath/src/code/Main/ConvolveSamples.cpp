/*
  ConvolveSamples.cpp

  convolve, or take the cross product, of two sample sets of the same dimension

  Li-Yi Wei
  11/23/2013

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "PlainBoxDomain.hpp"
#include "ToroidalBoxDomain.hpp"
#include "Math.hpp"
#include "Exception.hpp"

Coordinate Shuffle(const Coordinate & input, const vector<int> & match)
{
    Coordinate output(input);
    output.Clear(0);

    for(unsigned int i = 0; i < match.size(); i++)
    {
        const int index = match[i];

        if((index >= 0) && (index < input.Dimension()))
        {
            output[i] = input[index];
        }
    }

    return output;
}

Sample Shuffle(const Sample & input, const vector<int> & match)
{
    Sample output(input);

    output.coordinate = Shuffle(input.coordinate, match);

    return output;
}

float Distance2(const Sample & sample1, const Sample & sample2, const vector<int> & match_dim1, const vector<int> & match_dim2, const Domain & domain)
{
    const Sample shuffle1 = Shuffle(sample1, match_dim1);
    const Sample shuffle2 = Shuffle(sample2, match_dim2);

    Sample nearest;

    if(! domain.Nearest(shuffle1, shuffle2, nearest))
    {
        return Math::INF;
    }
    else
    {
        return Utility::Distance2(shuffle1.coordinate, nearest.coordinate);
    }
}

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 5;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_set1 input_set2 dimension scheme [boundary (0 none, 1 toroidal) domain_size]" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_sample_file_name1 = argv[++arg_ctr];
    const string input_sample_file_name2 = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const string scheme = argv[++arg_ctr];

    vector< vector<int> > match_dimensions(2, vector<int>(dimension));

    int boundary = 0;

    vector<float> domain_size(dimension);
    
    if(scheme != "all")
    {
        if(argc < (min_argc + 3*dimension + 1))
        {
            cerr << argv[0] << " not enough input arguments for scheme " << scheme << endl;
            return 1;
        }

        for(unsigned int i = 0; i < match_dimensions.size(); i++)
        {
            for(unsigned int j = 0; j < match_dimensions[i].size(); j++)
            {
                match_dimensions[i][j] = atoi(argv[++arg_ctr]);
            }
        }

        boundary = atoi(argv[++arg_ctr]);

        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            domain_size[i] = atof(argv[++arg_ctr]);
        }
    }

    PlainBoxDomain plain_domain(domain_size);
    ToroidalBoxDomain toroidal_domain(domain_size);

    const Domain * p_plain_domain = &plain_domain;
    const Domain * p_toroidal_domain = &toroidal_domain;
    const Domain * p_domain = (boundary ? p_toroidal_domain : p_plain_domain);
    const Domain & domain = *p_domain;

    // input samples
    vector<Sample> input_set1;
    if(! Utility::ReadSamples(dimension, input_sample_file_name1, input_set1))
    {
        cerr << "cannot read samples from " << input_sample_file_name1 << endl;
        return 1;
    }
    
    vector<Sample> input_set2;
    if(! Utility::ReadSamples(dimension, input_sample_file_name2, input_set2))
    {
        cerr << "cannot read samples from " << input_sample_file_name2 << endl;
        return 1;
    }

    // process
    vector<Sample> output_set;
    Sample sample;
    for(unsigned int i = 0; i < input_set1.size(); i++)
    {
        const Sample & sample1 = input_set1[i];

        sample = sample1;

        float min_dist = Math::INF;
        int min_index = -1;

        for(unsigned int j = 0; j < input_set2.size(); j++)
        {
            const Sample & sample2 = input_set2[j];

            if(scheme == "nearest")
            {
                const float dist = Distance2(sample1, sample2, match_dimensions[0], match_dimensions[1], domain);

                if(dist < min_dist)
                {
                    min_dist = dist;
                    min_index = j;
                }
            }
            else // assume scheme == "all"
            {
                sample.coordinate = Utility::Add(sample1.coordinate, sample2.coordinate);

                output_set.push_back(sample);
            }
        } 

        if(scheme == "nearest")
        {
            if((min_index >= 0) && (min_index < input_set2.size()))
            {
                const Sample & sample2 = input_set2[min_index];

                sample.coordinate = Utility::Add(sample1.coordinate, sample2.coordinate);

                output_set.push_back(sample);
            }
        }    
    }

    // output
    if(! Utility::WriteSamples(output_set, "cout"))
    {
        cerr << "cannot write samples to " << "cout" << endl;
        return 1;
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
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
