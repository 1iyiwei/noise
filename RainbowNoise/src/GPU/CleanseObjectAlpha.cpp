/*
  CleanseObjectAlpha.cpp

  use thresholding to cleanup the alpha of an object image

  Li-Yi Wei
  10/18/2008

*/

  
#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"

int Threshold(const int threshold, const int target, const int value, const int down_or_up)
{
    if((down_or_up && (threshold < value) && (value < target)) || (!down_or_up && (target < value) && (value < threshold)))
    {
        return target;
    }
    else
    {
        return value;
    }
}

int Main(int argc, char **argv)
{
    const int min_argc = 7;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_file_name output_file_name threshold_r threshold_g threshold_b target" << endl;
        return 1;
    }

    int argCtr = 0;

    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];

    FrameBuffer::P3 threshold;
    threshold.r = atoi(argv[++argCtr]);
    threshold.g = atoi(argv[++argCtr]);
    threshold.b = atoi(argv[++argCtr]);

    FrameBuffer::P3 target;
    target.r = target.r = target.b = atoi(argv[++argCtr]);

    int down_or_up = -1;

    if((threshold.r >= target.r) && (threshold.g >= target.g) && (threshold.b >= target.b)) down_or_up = 0;
    if((threshold.r <= target.r) && (threshold.g <= target.g) && (threshold.b <= target.b)) down_or_up = 1;

    if((down_or_up < 0) || (down_or_up > 1))
    {
        cerr << "illegal relationship between threshold and target " << endl;
        return 1;
    }

    // input
    Array<FrameBuffer::P3> image;
    int max_value;
    if(! FrameBuffer::ReadPPM(input_file_name, image, max_value))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 0;
    }

    // process
    vector <FrameBuffer::P3> pixels;
    image.Get(pixels);

    for(unsigned int i = 0; i < pixels.size(); i++)
    {
        pixels[i].r = Threshold(threshold.r, target.r, pixels[i].r, down_or_up);
        pixels[i].g = Threshold(threshold.g, target.g, pixels[i].g, down_or_up);
        pixels[i].b = Threshold(threshold.b, target.b, pixels[i].b, down_or_up);
    }

    image.Put(pixels);

    // output
    if(! FrameBuffer::WritePPM(image, max_value, output_file_name))
    {
        cerr << "cannot write to " << output_file_name << endl;
        return 0;
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
