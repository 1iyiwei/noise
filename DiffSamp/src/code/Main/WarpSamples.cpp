/*
  WarpSamples.cpp

  Li-Yi Wei
  10/16/2010

*/

#include <iostream>
#include <string>
using namespace std;

#include "MatrixWarp.hpp"
#include "TerrainWarp.hpp"
#include "HaarWarp.hpp"

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 5;

    if(argc < min_argc)
    {
        cerr << argv[0] << " warp_option forward_or_inverse (0/1) dimension input_sample_file_name [haarwarp: importance cell_size] " << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string warp_option = argv[++arg_ctr];
    const int forward_inverse = atoi(argv[++arg_ctr]);
    const int dimension = atoi(argv[++arg_ctr]);
    const string input_sample_file_name = argv[++arg_ctr];

    // input warp

    Matrix * p_matrix = 0;
    Warp * p_warp = 0;

    if(warp_option == "terrain")
    {
        p_warp = new TerrainWarp;
    }
    else if(warp_option == "haar")
    {
        if((arg_ctr + 2) >= argc)
        {
            cerr << "not enough input arguments for haar warp" << endl;
            return 1;
        }

        const string importance_file_name = argv[++arg_ctr];
        const float cell_size = atof(argv[++arg_ctr]);

        Array<FrameBuffer::F2> importance;
        if(!FrameBuffer::ReadF2M(importance_file_name, importance))
        {
            cerr << "cannot read from " << importance_file_name << endl;
            return 1;
        }

        p_warp = new HaarWarp(importance, cell_size);
    }
    else
    {
        // warp matrix
        Array<FrameBuffer::F2> input;
        if(!FrameBuffer::ReadF2M(warp_option, input))
        {
            cerr << "cannot read from " << warp_option << endl;
            return 1;
        }

        p_matrix = new Matrix(input);

        p_warp = new MatrixWarp(*p_matrix);
    }

    if(!p_warp) throw Exception("null warp");

    const Warp & warp = *p_warp;

    // input samples
    vector<Sample> input_samples;
    if(! Utility::ReadSamples(dimension, input_sample_file_name, input_samples))
    {
        cerr << "cannot read samples from " << input_sample_file_name << endl;
        return 1;
    }
    
    // process
    vector<Sample> output_samples(input_samples.size());

    for(unsigned int i = 0; i < output_samples.size(); i++)
    {
        output_samples[i] = input_samples[i];

        int status = 0;
        if(forward_inverse == 0)
        {
            status = warp.ForwardMap(input_samples[i].coordinate, output_samples[i].coordinate);
        }
        else
        {
            status = warp.InverseMap(input_samples[i].coordinate, output_samples[i].coordinate);
        }

        if(! status)
        {
            cerr << "cannot warp sample" << endl;
            return 1;
        }
    }

    // output
    if(! Utility::WriteSamples(output_samples, "cout"))
    {
        cerr << "cannot write samples to " << "cout" << endl;
        return 1;
    }

    // clean up
    if(! p_warp)
    {
        delete p_warp;
        p_warp = 0;
    }

    if(! p_matrix)
    {
        delete p_matrix;
        p_matrix = 0;
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
