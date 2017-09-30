/*
  F2M2PFM.cpp

  conversion from F2M to PFM

  Li-Yi Wei
  08/31/2010

*/

#include <iostream>
#include <string>
using namespace std;

#include "FrameBuffer.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    const int argc_min = 3;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " input_file_name output_file_name" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const string output_file_name = argv[++arg_ctr];

    Array<FrameBuffer::F2> input;
    if(!FrameBuffer::ReadF2M(input_file_name, input))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    vector<int> size;
    input.Size(size);

    Array<FrameBuffer::PF> output(size);

    {
        vector<FrameBuffer::F2> input_pixels;
        input.Get(input_pixels);

        vector<FrameBuffer::PF> output_pixels;
        output.Get(output_pixels);

        if(input_pixels.size() != output_pixels.size()) throw Exception("size mismatch");

        for(unsigned int i = 0; i < output_pixels.size(); i++)
        {
            output_pixels[i].r = input_pixels[i];
            output_pixels[i].g = input_pixels[i];
            output_pixels[i].b = input_pixels[i];
        }

        output.Put(output_pixels);
    }

    if(!FrameBuffer::WritePFM(output, output_file_name))
    {
        cerr << "cannot write to " << output_file_name << endl;
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
