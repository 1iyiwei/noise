/*
  Rui2F2M.cpp

  conversion from Rui's pfm to F2M

  Li-Yi Wei
  01/12/2011

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
    if(!FrameBuffer::ReadF2MB(input_file_name, input))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

#if 1 // debug
    vector<int> size;
    input.Size(size);

    cerr << "Rui size:";
    for(unsigned int i = 0; i < size.size(); i++)
    {
        cerr << " " << size[i];
    }
    cerr << endl;
#endif

    if(!FrameBuffer::WriteF2M(input, output_file_name))
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
