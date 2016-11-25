/*
  AnisotropicWhiteNoise.cpp

  Li-Yi Wei
  10/16/2010

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <memory>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "PlainBoxDomain.hpp"
#include "MatrixWarp.hpp"
#include "SampledJacobianField.hpp"

#include "AnisotropicWhiteNoise.hpp"

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Random.hpp"
#include "Exception.hpp"

#define _FACTOR_RADIUS_INTO_JACOBIAN

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 7;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " jacobian_field dimension num_samples domain_size (dimension numbers) radius (little sphere on the output domain) cell_size" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string jacobian_field_name = argv[++arg_ctr];
    
    const int dimension = atof(argv[++arg_ctr]);

    const int num_samples = atoi(argv[++arg_ctr]);

    if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension, 1.0);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }
    
    const float radius = atof(argv[++arg_ctr]);
    const float cell_size = atof(argv[++arg_ctr]);

    // init random
    Random::InitRandomNumberGenerator();

    // jacobian field
    Array<Matrix> jacobians;
    if(! Utility::ReadMatrixArray(jacobian_field_name, jacobians))
    {
        cerr << "cannot read from " << jacobian_field_name << endl;
        return 1;
    }

    PlainBoxDomain domain(domain_size);
    
#ifdef _FACTOR_RADIUS_INTO_JACOBIAN
    SampledJacobianField jacobian_field(domain, jacobians, cell_size, radius);    
    AnisotropicWhiteNoise white_noise(jacobian_field, radius); // has to use radius regardless of the one using above
#else
    SampledJacobianField jacobian_field(domain, jacobians, cell_size, 0);    
    AnisotropicWhiteNoise white_noise(jacobian_field, radius);
#endif

#if 0
    cerr << "forward: " << jacobian_field.ForwardMeanEigenValue() << ", " << jacobian_field.ForwardMaxEigenValue() << ", " << jacobian_field.ForwardMinEigenValue() << endl;
    cerr << "inverse: " << jacobian_field.InverseMeanEigenValue() << ", " << jacobian_field.InverseMaxEigenValue() << ", " << jacobian_field.InverseMinEigenValue() << endl;
#endif

    // samples
    vector<Sample> samples;
    Sample sample(dimension);

    for(int which_sample = 0; which_sample < num_samples; which_sample++)
    {
        if(! white_noise.Get(sample))
        {
            cerr << "cannot get anisotropic white noise sample" << endl;
            return 1;
        }
        
        samples.push_back(sample);
    }

    // output
    string output_file_name;

    if((arg_ctr + 1) < argc)
    {
        output_file_name = argv[++arg_ctr];
    }
    else
    {
        output_file_name = "cout";
    }

    if(! Utility::WriteSamples(samples, output_file_name))
    {
        cerr << "cannot write samples to " << output_file_name << endl;
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
