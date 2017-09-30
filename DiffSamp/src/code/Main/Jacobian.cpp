/*
  Jacobian.cpp

  a collection of Jacobian related operations

  Li-Yi Wei
  10/18/2010

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>

#include "MatrixWarp.hpp"
#include "Random.hpp"
#include "UniformWhiteNoise.hpp"
#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int WarpTest(int argc, char **argv)
{
    // input arguments
    int min_argc = 2;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_matrix" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_matrix_name = argv[++arg_ctr];

    Array<float> input;
    if(! FrameBuffer::ReadF2M(input_matrix_name, input))
    {
        cerr << "cannot read from " << input_matrix_name << endl;
    }

    // matrix
    Matrix input_matrix(input);
    vector<float> domain_size(input_matrix.Size(1)-1, 1);

    // matrix warp
    MatrixWarp warp(input_matrix);

    // random source
    Random::InitRandomNumberGenerator();

    UniformWhiteNoise white_noise(domain_size);

    Sample sample;
    
    if(! white_noise.Get(sample)) throw Exception("cannot get white noise sample");

    Coordinate forward_coordinate = sample.coordinate;
    Coordinate inverse_coordinate;
    if(! warp.InverseMap(forward_coordinate, inverse_coordinate)) throw Exception("cannot inverse map");

    Matrix forward_jacobian, inverse_jacobian;

    if(! warp.ForwardJacobian(forward_coordinate, forward_jacobian)) throw Exception("cannot get forward jacobian");
    if(! warp.InverseJacobian(forward_coordinate, inverse_jacobian)) throw Exception("cannot get inverse jacobian");

    Matrix identity;
    if(! forward_jacobian.Multiply(inverse_jacobian, identity)) throw Exception("cannot multiply matrix");

    // display
    vector<int> index(2);
    float value = 0;
    for(index[0] = 0; index[0] < identity.Size(0); index[0]++)
    {
        for(index[1] = 0; index[1] < identity.Size(1); index[1]++)
        {
            float value;
            if(! identity.Get(index, value)) throw Exception("cannot get identity value");

            cerr << value << " ";
        }
        cerr << endl;
    }

    // done
    return 0;
}

int Query(int argc, char **argv)
{
    // input arguments
    int min_argc = 4;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_matrix option (forward/inverse) coordinate" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_matrix_name = argv[++arg_ctr];
    const string option = argv[++arg_ctr];

    Array<float> input;
    if(! FrameBuffer::ReadF2M(input_matrix_name, input))
    {
        cerr << "cannot read from " << input_matrix_name << endl;
    }

    // matrix
    Matrix input_matrix(input);

    const int dimension = input_matrix.Size(1)-1;

    vector<float> domain_size(dimension, 1);

    // matrix warp
    MatrixWarp warp(input_matrix);

    if(argc < (min_argc + dimension -1))
    {
        cerr << argc << " < " << "(" << min_argc << " + " << dimension << " - 1)" << endl;
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    Coordinate query(dimension);
    for(int i = 0; i < query.Dimension(); i++)
    {
        query[i] = atof(argv[++arg_ctr]);
    }

    Matrix jacobian;

    if(option == "forward")
    {
        if(! warp.ForwardJacobian(query, jacobian))
        {
            cerr << "cannot get forward jacobian " << endl;
            return 1;
        }
    }
    else
    {
        if(! warp.InverseJacobian(query, jacobian))
        {
            cerr << "cannot get inverse jacobian " << endl;
            return 1;
        }
    }

    // display
    float value = 0;
    vector<int> index(2);
    for(index[0] = 0; index[0] < jacobian.Size(0); index[0]++)
    {
        for(index[1] = 0; index[1] < jacobian.Size(1); index[1]++)
        {
            jacobian.Get(index, value);
            cerr << value << " ";
        }
        cerr << endl;
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

        int arg_ctr = 0;
        const string option = argv[++arg_ctr];

        if(option == "warptest")
        {
            return WarpTest(argc-1, &argv[1]);
        }
        else if(option == "query")
        {
            return Query(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown matrix option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
