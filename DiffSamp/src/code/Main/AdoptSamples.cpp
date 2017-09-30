/*
  AdoptSamples.cpp

  Li-Yi Wei
  05/09/2011

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "PlainBoxDomain.hpp"
#include "ToroidalBoxDomain.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 5;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_sample_file_name dimension domain_type (plain or toroidal) domain_size " << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_sample_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const string domain_type = argv[++arg_ctr];

    if(argc < (min_argc + dimension -1))
    {
        cerr << "insufficient arguments for shift amount" << endl;
        return 1;
    }

    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }

    // domain
    Domain * p_domain = 0;
    if(domain_type == "plain")
    {
        p_domain = new PlainBoxDomain(domain_size);
    }
    else if(domain_type == "toroidal")
    {
        p_domain = new ToroidalBoxDomain(domain_size);
    }
    else
    {
        cerr << "unknown domain type" << endl;
        return 1;
    }

    if(!p_domain) throw Exception("null domain");
    const Domain & domain = *p_domain;

    // input samples
    vector<Sample> input_samples;
    if(! Utility::ReadSamples(dimension, input_sample_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_sample_file_name << endl;
        return 1;
    }
    
    // process
    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        Sample & sample = input_samples[i];

        if(! domain.Adopt(sample, sample))
        {
            cerr << "cannot adopt sample" << endl;
            return 1;
        }
    }

    // output
    if(! Utility::WriteSamples(input_samples, "cout"))
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
