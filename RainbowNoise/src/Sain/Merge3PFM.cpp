/*
  Merge3PFM.cpp

  combine 3 pfm into a colorful output

  Li-Yi Wei
  10/07/2008

*/

#include <iostream>
using namespace std;

#include "Exception.hpp"
#include "FrameBuffer.hpp"

int Main(int argc, char **argv)
{
    const int min_argc = 5;
    
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " input_r input_g input_b output" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_r_file_name = argv[++argCtr];
    const char * input_g_file_name = argv[++argCtr];
    const char * input_b_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];

    // read input
    Array<FrameBuffer::PF> input_r, input_g, input_b;

    if(! FrameBuffer::ReadPFM(input_r_file_name, input_r))
    {
        cerr << "error in reading " << input_r_file_name << endl;
        return 1;
    } 
    if(! FrameBuffer::ReadPFM(input_g_file_name, input_g))
    {
        cerr << "error in reading " << input_g_file_name << endl;
        return 1;
    }
    if(! FrameBuffer::ReadPFM(input_b_file_name, input_b))
    {
        cerr << "error in reading " << input_b_file_name << endl;
        return 1;
    }

    vector<FrameBuffer::PF> input_r_values, input_g_values, input_b_values;
    
    input_r.Get(input_r_values);
    input_g.Get(input_g_values);
    input_b.Get(input_b_values);

    if((input_r_values.size() != input_g_values.size()) || (input_r_values.size() != input_b_values.size()))
    {
        cerr << "incompatible inputs" << endl;
        return 1;
    }

    Array<FrameBuffer::PF> output = input_r;
    vector<FrameBuffer::PF> output_values = input_r_values;

    for(unsigned int i = 0; i < output_values.size(); i++)
    {
        output_values[i].r = input_r_values[i].r;
        output_values[i].g = input_g_values[i].g;
        output_values[i].b = input_b_values[i].b;
    }

    if(!output.Put(output_values))
    {
        cerr << "cannot put output values" << endl;
        return 1;
    }

    if(! FrameBuffer::WritePFM(output, output_file_name))
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
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
