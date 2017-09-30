/*
  NormPFT.cpp

  normalize a power spectrum via white noise factorization

  Li-Yi Wei
  08/17/2010

*/

#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

#include <math.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"

#ifdef _USE_F2M
typedef FrameBuffer::F2 Pixel;
#else
typedef FrameBuffer::PF Pixel;
#endif

int Main(int argc, char **argv)
{
    if(argc < 6)
    {
        cerr << "Usage: " << argv[0] << " operator-option (subadd, subaddnonneg, div) invariance_constant input-pft-file-name reference-white-noise-pft-file-name output-pft_file-name" << endl;
        return 1;
    }
    
    // input arguments
    int arg_ctr = 0;
    const string operator_option = argv[++arg_ctr];
    const float invariance_const = atof(argv[++arg_ctr]);
    const string input_file_name = argv[++arg_ctr];
    const string reference_file_name = argv[++arg_ctr];
    const string output_file_name = argv[++arg_ctr];

    // read inputs
    Array<Pixel> input_spectrum;
#ifdef _USE_F2M
    if(! FrameBuffer::ReadF2M(input_file_name, input_spectrum))
#else
    if(! FrameBuffer::ReadPFM(input_file_name, input_spectrum))
#endif
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }

    Array<Pixel> reference_spectrum;
#ifdef _USE_F2M
    if(! FrameBuffer::ReadF2M(reference_file_name, reference_spectrum))
#else
    if(! FrameBuffer::ReadPFM(reference_file_name, reference_spectrum))
#endif
    {
        cerr << "error in reading " << reference_file_name << endl;
        return 1;
    }

    Array<Pixel> output_spectrum(input_spectrum);

    // check consistency
    if(input_spectrum.Dimension() != reference_spectrum.Dimension())
    {
        cerr << "incompatible dimension between input and reference" << endl;
        return 1;
    }

    const int dimension = input_spectrum.Dimension();
    
    for(int i = 0; i < dimension; i++)
    {
        if(input_spectrum.Size(i) != reference_spectrum.Size(i))
        {
            cerr << "incompatible size between input and reference" << endl;
            return 1;
        }
    }

    // operation
    vector<Pixel> input_pixels;
    input_spectrum.Get(input_pixels);

    vector<Pixel> reference_pixels;
    reference_spectrum.Get(reference_pixels);

    if(input_pixels.size() != reference_pixels.size())
    {
        cerr << "incompatible size between input and reference" << endl;
        return 1;
    }

    vector<Pixel> output_pixels(input_pixels);

    if(operator_option.find("subadd") != string::npos)
    {
        for(unsigned int i = 0; i < output_pixels.size(); i++)
        {
#ifdef _USE_F2M 
            output_pixels[i] = input_pixels[i] - reference_pixels[i] + invariance_const;
#else
            output_pixels[i].r = input_pixels[i].r - reference_pixels[i].r + invariance_const;
            output_pixels[i].g = input_pixels[i].g - reference_pixels[i].g + invariance_const;
            output_pixels[i].b = input_pixels[i].b - reference_pixels[i].b + invariance_const;
#endif
        }

        if(operator_option.find("nonneg") != string::npos)
        {
            for(unsigned int i = 0; i < output_pixels.size(); i++)
            {
#ifdef _USE_F2M 
                if(output_pixels[i] < 0) output_pixels[i] = 0;
#else
                if(output_pixels[i].r < 0) output_pixels[i].r = 0;
                if(output_pixels[i].g < 0) output_pixels[i].g = 0;
                if(output_pixels[i].b < 0) output_pixels[i].b = 0;
#endif
            }
        }
    }
    else if(operator_option.find("div") != string::npos)
    {
        for(unsigned int i = 0; i < output_pixels.size(); i++)
        {
#ifdef _USE_F2M
            output_pixels[i] = input_pixels[i]/reference_pixels[i]*invariance_const;
#else
            output_pixels[i].r = input_pixels[i].r/reference_pixels[i].r*invariance_const;
            output_pixels[i].g = input_pixels[i].g/reference_pixels[i].g*invariance_const;
            output_pixels[i].b = input_pixels[i].b/reference_pixels[i].b*invariance_const;
#endif
        }
    }
    else
    {
        cerr << "unknown operator" << endl;
        return 1;
    }

    output_spectrum.Put(output_pixels);

    // keep DC unchanged
    vector<int> output_size(dimension);
    output_spectrum.Size(output_size);

    vector<int> dc_index = output_size;
    for(unsigned int i = 0; i < dc_index.size(); i++)
    {   
        dc_index[i] = output_size[i]/2;
    }

    {
        Pixel value;
        if(! input_spectrum.Get(dc_index, value)) throw Exception("cannot get input dc value");
        if(! output_spectrum.Put(dc_index, value)) throw Exception("cannot put output dc value");
    }

    // output
#ifdef _USE_F2M
    if(! FrameBuffer::WriteF2M(output_spectrum, output_file_name))
#else
    if(! FrameBuffer::WritePFM(output_spectrum, output_file_name))
#endif
    {
        cerr << "error in writing " << output_file_name << endl;
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
