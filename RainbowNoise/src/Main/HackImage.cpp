/*
  HackImage.cpp

  various hacks to the (importance) image

  Li-Yi Wei
  10/02/2009

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "FrameBuffer.hpp"
#include "Exception.hpp"

int ClipMinimum(int argc, char **argv)
{
    // input arguments
    int min_argc = 6;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_rgb_file_name_ppm min_color_rgb output_file_name_ppm" << endl;
        return 1;
    }

    int arg_ctr = 0;
    
    const string input_file_name = argv[++arg_ctr];
    
    FrameBuffer::P3 min_color;
    min_color.r = atoi(argv[++arg_ctr]);
    min_color.g = atoi(argv[++arg_ctr]);
    min_color.b = atoi(argv[++arg_ctr]);

    const string output_file_name = argv[++arg_ctr];
    
    // input image
    Array<FrameBuffer::P3> input;
    int max_value = 0;
    if(! FrameBuffer::ReadPPM(input_file_name.c_str(), input, max_value))
    {
        cerr << "cannot read ppm from " << input_file_name << endl;
        return 1;
    }

    // pixels
    vector<FrameBuffer::P3> pixels;
    if(! input.Get(pixels)) throw Exception("cannot get pixels");

    // process
    for(unsigned int i = 0; i < pixels.size(); i++)
    {
        if(pixels[i].r < min_color.r) pixels[i].r = min_color.r;
        if(pixels[i].g < min_color.g) pixels[i].g = min_color.g;
        if(pixels[i].b < min_color.b) pixels[i].b = min_color.b;
    }

    if(! input.Put(pixels)) throw Exception("cannot put pixels");

    // output
    if(! FrameBuffer::WritePPM(input, max_value, output_file_name.c_str()))
    {
        cerr << "cannot write ppm to " << output_file_name << endl;
        return 1;
    }

    // done
    return 0;
}

int ClipMaximum(int argc, char **argv)
{
    // input arguments
    int min_argc = 6;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_rgb_file_name_ppm max_color_rgb output_file_name_ppm" << endl;
        return 1;
    }

    int arg_ctr = 0;
    
    const string input_file_name = argv[++arg_ctr];
    
    FrameBuffer::P3 max_color;
    max_color.r = atoi(argv[++arg_ctr]);
    max_color.g = atoi(argv[++arg_ctr]);
    max_color.b = atoi(argv[++arg_ctr]);

    const string output_file_name = argv[++arg_ctr];
    
    // input image
    Array<FrameBuffer::P3> input;
    int max_value = 0;
    if(! FrameBuffer::ReadPPM(input_file_name.c_str(), input, max_value))
    {
        cerr << "cannot read ppm from " << input_file_name << endl;
        return 1;
    }

    // pixels
    vector<FrameBuffer::P3> pixels;
    if(! input.Get(pixels)) throw Exception("cannot get pixels");

    // process
    for(unsigned int i = 0; i < pixels.size(); i++)
    {
        if(pixels[i].r > max_color.r) pixels[i].r = max_color.r;
        if(pixels[i].g > max_color.g) pixels[i].g = max_color.g;
        if(pixels[i].b > max_color.b) pixels[i].b = max_color.b;
    }

    if(! input.Put(pixels)) throw Exception("cannot put pixels");

    // output
    if(! FrameBuffer::WritePPM(input, max_value, output_file_name.c_str()))
    {
        cerr << "cannot write ppm to " << output_file_name << endl;
        return 1;
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

        if(option == "clipmin")
        {
            return ClipMinimum(argc-1, &argv[1]);
        } 
        else if(option == "clipmax")
        {
            return ClipMaximum(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown hack image option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
