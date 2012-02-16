/*
  ColorTransform.cpp

  Li-Yi Wei
  09/27/2009

*/

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "FairColorTransformer.hpp"
#include "FrameBuffer.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 3;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_rgb_file_name_pfm output_dimension output_basis (output_dimension vectors) output_file_names (output_dimension)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const char * input_file_name = argv[++arg_ctr];
    const int input_dimension = 3; // assuming RGB
    const int output_dimension = atoi(argv[++arg_ctr]);

    if(output_dimension <= 0)
    {
        throw Exception("output_dimension <= 0");
    }

    if(argc < (min_argc + input_dimension*output_dimension + output_dimension))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<ColorTransformer::Vector> output_basis(output_dimension);
    ColorTransformer::Vector basis_vector(input_dimension);

    for(unsigned int i = 0; i < output_basis.size(); i++)
    {
        for(unsigned int j = 0; j < basis_vector.size(); j++)
        {
            basis_vector[j] = atof(argv[++arg_ctr]);
        }

        output_basis[i] = basis_vector;
    }

    vector<string> output_file_names(output_dimension);

    for(unsigned int i = 0; i < output_file_names.size(); i++)
    {
        output_file_names[i] = argv[++arg_ctr];
    }

    // input image
    Array<FrameBuffer::PF> input;
    if(! FrameBuffer::ReadPFM(input_file_name, input))
    {
        cerr << "cannot read pfm from " << input_file_name << endl;
        return 1;
    }

    // output images
    vector< Array<FrameBuffer::PF> > outputs(output_dimension, input);
    
    // color transformer
    FairColorTransformer color_transformer(input_dimension, output_basis);

    // raw pixels
    vector<FrameBuffer::PF> input_pixels;
    if(! input.Get(input_pixels)) throw Exception("cannot get input pixels");
    vector< vector<FrameBuffer::PF> > output_pixels(output_dimension, input_pixels);
    
    // do work
    ColorTransformer::Vector input_vector(input_dimension);
    ColorTransformer::Vector output_vector(output_dimension);

    for(unsigned int i = 0; i < input_pixels.size(); i++)
    {
        input_vector[0] = input_pixels[i].r;
        input_vector[1] = input_pixels[i].g;
        input_vector[2] = input_pixels[i].b;

        if(! color_transformer.Transform(input_vector, output_vector))
        {
            throw Exception("error in color transform");
        }

        if(output_vector.size() != output_pixels.size())
        {
            throw Exception("output_vector.size() != output_pixels.size()");
        }

        for(unsigned int j = 0; j < output_pixels.size(); j++)
        {
            output_pixels[j][i].r = output_pixels[j][i].g = output_pixels[j][i].b = output_vector[j];
        }
    }

    // put back output pixels
    if(outputs.size() != output_pixels.size()) throw Exception("outputs.size() != output_pixels.size()");
    for(unsigned int i = 0; i < outputs.size(); i++)
    {
        if(! outputs[i].Put(output_pixels[i])) throw Exception("cannot put output pixels");
    }

    // output
    if(outputs.size() != output_file_names.size()) throw Exception("output.size() != output_file_names.size()");
    for(unsigned int i = 0; i < outputs.size(); i++)
    {
        if(! FrameBuffer::WritePFM(outputs[i], output_file_names[i].c_str()))
        {
            cerr << "cannot write to " << output_file_names[i] << endl;
            return 1;
        }
    }

#if 1 // debug
    {
        vector<float> error(input_vector.size(), 0);
        
        for(unsigned int i = 0; i < input_pixels.size(); i++)
        {
            input_vector[0] = input_pixels[i].r;
            input_vector[1] = input_pixels[i].g;
            input_vector[2] = input_pixels[i].b;

            for(unsigned int j = 0; j < output_pixels.size(); j++)
            {
                output_vector[j] = output_pixels[j][i].g;
            }

            for(unsigned int k = 0; k < input_vector.size(); k++)
            {
                float output_sum = 0;
             
                for(unsigned int j = 0; j < output_vector.size(); j++)
                {
                    output_sum += output_vector[j]*output_basis[j][k];
                }

                const float diff = fabs(input_vector[k] - output_sum);

                if(diff > error[k]) error[k] = diff;
            }
        }

        cerr << "error:";
        for(unsigned int k = 0; k < error.size(); k++)
        {
            cerr << " " << error[k];
        }
        cerr << endl;
    }
#endif

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
