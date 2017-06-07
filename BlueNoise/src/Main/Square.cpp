/*
  make an image square

  Li-Yi Wei
  03/19/2008

*/

#include <iostream>
using namespace std;

#include <stdlib.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"

int Main(int argc, char **argv)
{
    if(argc < 6)
    {
        cerr << "Usage: " << argv[0] << " input-file-name output-file-name background (r, g, b)" << endl;
        return 1;
    }
    
    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    FrameBuffer::P3 background;
    background.r = atoi(argv[++argCtr]);
    background.g = atoi(argv[++argCtr]);
    background.b = atoi(argv[++argCtr]);
    
    // read input
    Array<FrameBuffer::P3> input_image;
    int input_maximum_value;
    if(! FrameBuffer::ReadPPM(input_file_name, input_image, input_maximum_value))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }

    vector<int> input_size;
    input_image.Size(input_size);

    if(input_size.size() != 2)
    {
        cerr << "input not 2D" << endl;
        return 1;
    }

    const int max_size = (input_size[0] > input_size[1] ? input_size[0] : input_size[1]);

    vector<int> output_size(input_size.size(), max_size);

    Array<FrameBuffer::P3> output_image(output_size);
    output_image.Clear(background);

    const int row_start = (output_size[0] - input_size[0])/2;
    const int col_start = (output_size[1] - input_size[1])/2;
    
    vector<int> location(2);
    FrameBuffer::P3 value;
    for(int row = row_start; row < row_start + input_size[0]; row++)
        for(int col = col_start; col < col_start + input_size[1]; col++)
        {
            // input
            location[0] = row - row_start; location[1] = col - col_start;
            input_image.Get(location, value);

            // output
            location[0] = row; location[1] = col;
            output_image.Put(location, value);
        }
    
    if(! FrameBuffer::WritePPM(output_image, input_maximum_value, output_file_name))
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
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
