/*
  DownSampleDFTPeak.cpp

  keep the maximum value among child pixels when downsample
  
  Li-Yi Wei
  7/3/2007

*/


#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"

int Main(int argc, char **argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << "input-file-name output-file-name downsample-factor" << endl;
        return 1;
    }

    const char * input_file_name = argv[1];
    const char * output_file_name = argv[2];
    const int downsample_factor = atoi(argv[3]);

    vector< vector<FrameBuffer::P3> > input_image;
    int max_pixel_value = 0;
    
    if(! FrameBuffer::ReadPPM(input_file_name, input_image, max_pixel_value))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }
        
    vector< vector<FrameBuffer::P3> > output_image(input_image.size()/downsample_factor);
    {
        for(unsigned int i = 0; i < output_image.size(); i++)
        {
            output_image[i] = vector<FrameBuffer::P3>(input_image[i].size()/downsample_factor);
        }
    }

    {
        for(unsigned int i = 0; i < output_image.size(); i++)
            for(unsigned int j = 0; j < output_image[i].size(); j++)
            {
                int i_select_o = i*downsample_factor;
                int j_select_o = j*downsample_factor;

                int i_select = i_select_o;
                int j_select = j_select_o;
                
                for(int k = 0; k < downsample_factor; k++)
                    for(int m = 0; m < downsample_factor; m++)
                    {
                        if(input_image[i_select_o+k][j_select_o+m].g > input_image[i_select][j_select].g)
                        {
                            i_select = i_select_o+k;
                            j_select = j_select_o+m;
                        }
                    }

                output_image[i][j].r = input_image[i_select][j_select].r;
                output_image[i][j].g = input_image[i_select][j_select].g;
                output_image[i][j].b = input_image[i_select][j_select].b;
            }
    }

    if(! FrameBuffer::WritePPM(output_image, max_pixel_value, output_file_name))
    {
        throw Exception("error in writing file!");
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
