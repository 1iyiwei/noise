/*
  MinSampleSpacing.cpp

  measure the minimum spacing between all pairs of given samples

  Li-Yi Wei
  08/15/2009

*/

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "Sample.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "PlainBoxDomain.hpp"
#include "ToroidalBoxDomain.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 5;
    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " input_file_name dimension domain_size (dimension numbers) boundary_condition (0 for none, 1 for toroidal)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }
    
    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }
    
    const string boundary_condition = argv[++arg_ctr];

    // read samples
    vector<Sample> samples;
    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }
    
    // domain
    Domain * p_domain = 0;

    if(boundary_condition == "toroidal")
    {
        p_domain = new ToroidalBoxDomain(domain_size);
    }
    else
    {
        p_domain = new PlainBoxDomain(domain_size);
    }

    auto_ptr<Domain> auto_ptr_domain(p_domain);
    const Domain & domain = *auto_ptr_domain;

    // process
    float min_dist = Math::INF;
    Sample nearest;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        const Sample & s0 = samples[i];

        for(unsigned int j = i+1; j < samples.size(); j++)
        {
            const Sample & s1 = samples[j];

            if(!domain.Nearest(s0, s1, nearest)) throw Exception("cannot get nearest");

            const float dist = Distance2(s0, nearest);

            if(dist < min_dist)
            {
                min_dist = dist;
            }
        }
    }

    float volume = 1.0;
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        volume *= domain_size[i];
    }

    const float normalized_num_samples = samples.size()/volume;

    // report
    cout << "minimum (relative) distance: " << sqrt(min_dist)/Math::ComputeMaxMinDistance(dimension, normalized_num_samples) << endl;

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
