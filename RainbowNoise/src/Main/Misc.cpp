/*
  Misc.cpp

  some misc convenience routines

  Li-Yi Wei
  10/21/2008

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"

int ImageMean(int argc, char **argv)
{
    const int min_argc = 2;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_image_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];

    Array<FrameBuffer::P3> image;
    int max_value = 0;

    if(! FrameBuffer::ReadPPM(input_file_name, image, max_value))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    vector<FrameBuffer::P3> pixels;
    image.Get(pixels);
    FrameBuffer::PF average = {0, 0, 0};
    for(unsigned int i = 0; i < pixels.size(); i++)
    {
        average.r += pixels[i].r;
        average.g += pixels[i].g;
        average.b += pixels[i].b;
    }
    
    cout << "average color: (" << 1.0*average.r/pixels.size() << ", " << 1.0*average.g/pixels.size() << ", " << 1.0*average.b/pixels.size() << ")" << endl;

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

        int argCtr = 0;
        const string option = argv[++argCtr];

        if(option == "image_mean")
        {
            return ImageMean(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
