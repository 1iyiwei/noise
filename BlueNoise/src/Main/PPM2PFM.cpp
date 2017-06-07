/*
  PPM2PFM.cpp

  conversion from ppm to pfm

  Li-Yi Wei
  07/16/2007

*/

#include <iostream>
using namespace std;

#include <math.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"

int Main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " input-file-name output-file-name " << endl;
        return 1;
    }
    
    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];

    // read input
    Array<FrameBuffer::P3> input_ppm;
    int input_maximum_value;
    if(! FrameBuffer::ReadPPM(input_file_name, input_ppm, input_maximum_value))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }

    vector<int> image_size;
    input_ppm.Size(image_size);

    Array<FrameBuffer::PF> output_pfm(image_size);

    vector<FrameBuffer::P3> input_pixels;
    vector<FrameBuffer::PF> output_pixels;

    if(! input_ppm.Get(input_pixels))
    {
        cerr << "cannot get input pixels" << endl;
        return 1;
    }
    if(! output_pfm.Get(output_pixels))
    {
        cerr << "cannot get output pixels" << endl;
        return 1;
    }

    if(input_pixels.size() != output_pixels.size())
    {
        cerr << "input and output have different number of pixels" << endl;
        return 1;
    }

    for(unsigned int i = 0; i < output_pixels.size(); i++)
    {
        output_pixels[i].r = input_pixels[i].r*1.0/input_maximum_value;
        output_pixels[i].g = input_pixels[i].g*1.0/input_maximum_value;
        output_pixels[i].b = input_pixels[i].b*1.0/input_maximum_value;

#if 0
        // inversion
        output_pixels[i].r = 1 - output_pixels[i].r;
        output_pixels[i].g = 1 - output_pixels[i].g;
        output_pixels[i].b = 1 - output_pixels[i].b;
#endif
    }

    if(! output_pfm.Put(output_pixels))
    {
        cerr << "cannot put output pixels" << endl;
        return 1;
    }

    if(! FrameBuffer::WritePFM(output_pfm, output_file_name))
    {
        cerr << "cannot write " << output_file_name << endl;
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
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
