/*
  FlyingZonePlate.cpp

  motion blur demo using a flying zone plate
  sin((x-vt)^2 + (y-vt)^2)
  
  Li-Yi Wei
  09/11/2007
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
#include "FrameBuffer.hpp"

int Main(int argc, char **argv)
{
    if(argc < 8)
    {
        cerr << "Usage: " << argv[0] << " dimension velocity sample_file_name output_file_name function_scale output_resolution filter_gaussian_std" << endl;

        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float velocity = atof(argv[++argCtr]);
    const char *sample_file_name = argv[++argCtr];
    const char *output_file_name = argv[++argCtr];
    const float function_scale = atof(argv[++argCtr]);
    const int output_resolution = atoi(argv[++argCtr]);
    const float filter_gaussian_std = atof(argv[++argCtr]);
    const int filter_support_scale = 1; //10
        
    if(dimension > 3)
    {
        cerr << "can only handle 2D and 3D now!" << endl;
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
        Sample sample(3);
        
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
        const float t = samples[i].coordinate[2];

        const float x_in = (x-velocity*t)/function_scale;
        const float y_in = (y-velocity*t)/function_scale;
        
        samples[i].value = sin(x_in*x_in + y_in*y_in);

#if 0
        const float threshold = 0.25*output_resolution;
        const float x_diff = x - velocity*t;
        const float y_diff = y - velocity*t;

        // cerr << "(" << x << ", " << y << ", " << t << " -> (" << x_diff << ", " << y_diff << ") " << threshold << endl;
        if((x_diff >= -threshold) && (x_diff <= threshold) && (y_diff >= -threshold) && (y_diff <= threshold))
        {
            samples[i].value = sin(x_in*x_in + y_in*y_in);
        }
        else
        {
            samples[i].value = 0;
        }
#endif
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
    vector< vector<FrameBuffer::P3> > output_image(output_values.size());

    for(unsigned int i = 0; i < output_image.size(); i++)
    {
        output_image[i] = vector<FrameBuffer::P3>(output_values[i].size());

        for(unsigned int j = 0; j < output_image[i].size(); j++)
        {
            const float value = output_values[i][j].weight > 0 ? output_values[i][j].value/output_values[i][j].weight : 0;
            
            output_image[i][j].r = output_image[i][j].g = output_image[i][j].b = static_cast<int>(floor((value+1)/2.0*255));
        }
    }

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
