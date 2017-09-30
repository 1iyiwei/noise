/*
  PGM2F2M.cpp

  conversion from PGM to F2M

  Li-Yi Wei
  10/13/2010

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

    Array<FrameBuffer::P2> input;
    int max_value = 0;
    if(!FrameBuffer::ReadPGM(input_file_name, input, max_value))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    vector<int> size;
    input.Size(size);

    Array<FrameBuffer::F2> output(size);

    {
        vector<FrameBuffer::P2> input_pixels;
        input.Get(input_pixels);

        vector<FrameBuffer::F2> output_pixels;
        output.Get(output_pixels);

        if(input_pixels.size() != output_pixels.size()) throw Exception("size mismatch");

        for(unsigned int i = 0; i < output_pixels.size(); i++)
        {
            output_pixels[i] = input_pixels[i];
        }

        output.Put(output_pixels);
    }

    if(!FrameBuffer::WriteF2M(output, output_file_name))
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
