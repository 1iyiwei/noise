/*
  ExtractDiscreteSamples.cpp

  generate discrete samples out of an image, e.g. produced by halftoning

  Li-Yi Wei
  04/28/2009

*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#include <stdlib.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "Sample.hpp"
#include "Array.hpp"
#include "SequentialCounter.hpp"

int Equal(const FrameBuffer::P3 & v1, const FrameBuffer::P3 & v2)
{
    return ((v1.r == v2.r) && (v1.g == v2.g) && (v1.b == v2.b));
}

vector<FrameBuffer::P3> BuildPalette(const string color_palette_file_name)
{
    vector<FrameBuffer::P3> palette;

    {
        ifstream input(color_palette_file_name.c_str());

        if(! input)
        {
            cerr << "cannot read from " << color_palette_file_name << endl;
            throw Exception("cannot read from palette");
        }
        else
        {
            FrameBuffer::P3 color;
            while(input.good())
            {
                input >> color.r;
                input >> color.g;
                input >> color.b;

                if(input.good()) palette.push_back(color);
            }
        }
    }

    return palette;
}

int Main(int argc, char **argv)
{
    // input arguments
    const int min_argc = 6;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_image_file_name num_pixels_per_unit_domain background_color_rgb [optional: palette_file_name]" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_image_file_name = argv[++arg_ctr];
    const int num_pixels_per_unit_domain = atoi(argv[++arg_ctr]);
    FrameBuffer::P3 background_color;
    background_color.r = atoi(argv[++arg_ctr]);
    background_color.g = atoi(argv[++arg_ctr]);
    background_color.b = atoi(argv[++arg_ctr]);

    const string palette_file_name = (arg_ctr < argc-1 ? argv[++arg_ctr] : "");

    const double pixel_size = 1.0/num_pixels_per_unit_domain;

    Array<FrameBuffer::P3> pixels;
    int max_value = 0;
    if(! FrameBuffer::ReadPPM(input_image_file_name.c_str(), pixels, max_value))
    {
        cerr << "cannot read from " << input_image_file_name << endl;
        return 1;
    }

    // processing
    vector<FrameBuffer::P3> palette;
    if(palette_file_name != "")
    {
        palette = BuildPalette(palette_file_name);
    }

    vector<Sample> samples;

    vector<int> size;
    pixels.Size(size);
    
    vector<int> digit_max(size);
    for(unsigned int i = 0; i < digit_max.size(); i++)
    {
        digit_max[i] = size[i]-1;
    }

    const int dimension = digit_max.size();
    SequentialCounter counter(dimension, vector<int>(dimension, 0), digit_max);
    counter.Reset();

    vector<int> index;
    FrameBuffer::P3 pixel;
    Sample sample(dimension);
    do
    {
        counter.Get(index);

        if(! pixels.Get(index, pixel)) throw Exception("cannot get pixel");
        
        if(Equal(pixel, background_color)) continue;

        int found_index = -1;
        for(unsigned int i = 0; i < palette.size(); i++)
        {
            if(Equal(palette[i], pixel))
            {
                if(found_index < 0)
                {
                    found_index = i;
                }
                else
                {
                    throw Exception("duplicated entries in palette");
                }
            }
        }

        if(found_index < 0)
        {
            if(palette_file_name == "")
            {
                palette.push_back(pixel);
                found_index = palette.size()-1;
            }
            else
            {
                cerr << "color not present in palette" << endl;
                return 1;
            }
        }

        sample.id = found_index;
        for(int i = 0; i < sample.coordinate.Dimension(); i++)
        {
            sample.coordinate[i] = (index[i] + 0.5)*pixel_size;
        }

        samples.push_back(sample);
    }
    while(counter.Next());

    // output
    cerr << samples.size() << " of samples in " << palette.size() << " classes extracted from " << input_image_file_name << endl;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        const Sample & sample = samples[i];

        cout << sample.id;
        for(int j = 0; j < sample.coordinate.Dimension(); j++)
        {
            cout << " " << sample.coordinate[j];
        }
        cout << endl;
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
