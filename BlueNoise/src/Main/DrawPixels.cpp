/*
  DrawPixels.cpp

  Li-Yi Wei
  09/16/2007

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"

int Main(int argc, char **argv)
{
    if(argc < 8)
    {
        cerr << "Usage: " << argv[0] << " input_file_name output_file_name color_threshold region (x_min, x_max, y_min, y_max)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int threshold = atoi(argv[++argCtr]);
    const int x_min = atoi(argv[++argCtr]);
    const int x_max = atoi(argv[++argCtr]);
    const int y_min = atoi(argv[++argCtr]);
    const int y_max = atoi(argv[++argCtr]);

    // read input image
    vector< vector<FrameBuffer::P3> > pixels;
    int maximum_value = 0;
    if(! FrameBuffer::ReadPPM(input_file_name, pixels, maximum_value))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }
    
    // select pixels
    vector<FrameBuffer::P3L2I> samples;

    for(int row = max(0, y_min); row < min(static_cast<int>(pixels.size()), y_max); row++)
    {
        for(int col = max(0, x_min); col < min(static_cast<int>(pixels[row].size()), x_max); col++)
        {
            if((pixels[row][col].r >= threshold) || (pixels[row][col].g >= threshold) || (pixels[row][col].b >= threshold))
            {
                FrameBuffer::P3L2I sample;

                sample.r = pixels[row][col].r;
                sample.g = pixels[row][col].g;
                sample.b = pixels[row][col].b;
                sample.x = col;
                sample.y = row;

                samples.push_back(sample);
            }
        }
    }
    
    // run
    vector<int> region(4);
    region[0] = x_min;
    region[1] = x_max;
    region[2] = y_min;
    region[3] = y_max;
    
    if(! FrameBuffer::WriteFIG(samples, region, output_file_name))
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
