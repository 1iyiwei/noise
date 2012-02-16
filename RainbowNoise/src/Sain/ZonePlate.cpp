/*
  ZonePlate.cpp

  sin((x)^2 + (y)^2)
  
  Li-Yi Wei
  09/11/2007
  09/07/2008 modified to remove motion

*/

#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Sample.hpp"
#include "SampleFunction.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "SequentialCounter.hpp"
#include "FrameBuffer.hpp"

int Main(int argc, char **argv)
{
    if(argc < 8)
    {
        cerr << "Usage: " << argv[0] << " dimension sample_file_name output_file_name function_offset function_scale output_resolution filter_gaussian_std" << endl;

        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const char *sample_file_name = argv[++argCtr];
    const char *output_file_name = argv[++argCtr];
    const float function_offset = atof(argv[++argCtr]);
    const float function_scale = atof(argv[++argCtr]);
    const int output_resolution = atoi(argv[++argCtr]);
    const float filter_gaussian_std = atof(argv[++argCtr]);
    const int filter_support_scale = 1; //10
        
    if(dimension != 2)
    {
        cerr << "can only handle 2D now!" << endl;
        return 1;
    }
    
    // read in samples
    ifstream input(sample_file_name);

    // read in samples
    cerr << "read in samples " << endl;
    
    vector<Sample> samples;
    
    if(!input)
    {
        return 0;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
#ifdef _SFT_READ_VALUE
            input >> sample.value;
#endif
            for(int i = 0; i < dimension; i++)
            {
                sample.coordinate[i] = 0;
            }
            
            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
                sample.coordinate[i] *= output_resolution;
            }

            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    cerr << "total " << samples.size() << " samples read" << endl;

    // evaluate samples
    cerr << "evaluate samples" << endl;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        const float x = samples[i].coordinate[0];
        const float y = samples[i].coordinate[1];

        const float x_in = (x + function_offset)/function_scale;
        const float y_in = (y + function_offset)/function_scale;
        
        samples[i].value = sin(x_in*x_in + y_in*y_in);
    }

    // splat samples to output image
    cerr << "splat samples" << endl;
    const int output_height = output_resolution;
    const int output_width = output_resolution;
    
    const int filter_radius = static_cast<int>(ceil(filter_gaussian_std*filter_support_scale));
    
    vector< vector<SampleFunction::Value> > output_values(output_height);
    for(unsigned int i = 0; i < output_values.size(); i++)
    {
        output_values[i] = vector<SampleFunction::Value>(output_width);
    }

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        const float value = samples[i].value;
        
        const float sample_x = samples[i].coordinate[0];
        const float sample_y = samples[i].coordinate[1];

        const int x_center = floor(sample_x+0.5);
        const int y_center = floor(sample_y+0.5);
        
        const int x_min = max(0, x_center - filter_radius);
        const int x_max = min(output_width-1, x_center + filter_radius);
        const int y_min = max(0, y_center - filter_radius);
        const int y_max = min(output_height-1, y_center + filter_radius);

        for(int x = x_min; x <= x_max; x++)
            for(int y = y_min; y <= y_max; y++)
            {
                const float weight = exp(-((x-sample_x)*(x-sample_x)+(y-sample_y)*(y-sample_y))/(filter_gaussian_std*filter_gaussian_std));
                
                output_values[x][y].weight += weight;
                output_values[x][y].value += weight*value;
            }
    }

    // dump output image
    vector<int> output_size(2);
    output_size[0] = output_height; output_size[1] = output_width;
    Array<FrameBuffer::P3> output_image(output_size);

    vector<int> output_size_minus1(2);
    for(unsigned int i = 0; i < output_size_minus1.size(); i++)
    {
        output_size_minus1[i] = output_size[i] - 1;
    }
    SequentialCounter counter(2, vector<int>(2, 0), output_size_minus1);
    counter.Reset();

    vector<int> index(2);
    do
    {
        counter.Get(index);
        const SampleFunction::Value output_value = output_values[index[0]][index[1]];

        const float value = output_value.weight > 0 ? output_value.value/output_value.weight : 0;
        
        FrameBuffer::P3 final_value;
        final_value.r = final_value.g = final_value.b = static_cast<int>(floor((value+1)/2.0*255));

        if(! output_image.Put(index, final_value))
        {
            cerr << "cannot put value into output_image" << endl;
            return 1;
        }
    }
    while(counter.Next());

    if(! FrameBuffer::WritePPM(output_image, 255, output_file_name))
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
        cerr << e.Message() << endl;
        return 1;
    }
}
