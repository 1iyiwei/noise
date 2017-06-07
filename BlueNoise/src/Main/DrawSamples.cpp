/*
  DrawSamples.cpp

  Li-Yi Wei
  6/18/2007

*/


#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "SFT.hpp"
#include "FrameBuffer.hpp"

int Main(int argc, char **argv)
{
    if(argc < 6)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name output-file-name dimension radius-for-fig/image-size-for-ppm border-for-fig/color-scheme-for-ppm (fig: 0 for no boundary, 1 for boundary, -2 for inverse color and no boundary, -1 for inverse color and boundary, ppm: 0 for white-dot-over-black-background, 1 vice versa) optional: domain_size (dimension numbers)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const float radius = atof(argv[++argCtr]);
    const int image_size = static_cast<int>(radius);
    const int color_scheme = atoi(argv[++argCtr]);
    vector<float> region(4);
    region[0] = 0;
    region[1] = argCtr+1 < argc ? atof(argv[++argCtr]) : -1;
    region[2] = 0;
    region[3] = argCtr+1 < argc ? atof(argv[++argCtr]) : -1;
    
    if(dimension != 2)
    {
        cerr << "currently only works for 2D" << endl;
        return 1;
    }
    
    ifstream input(input_file_name); // read in samples
    cout << "read in samples " << endl;
    
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
            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    cout << "total " << samples.size() << " samples read" << endl;
    
    // snap point samples to output image
    if(strstr(output_file_name, ".fig"))
    {
        vector<FrameBuffer::L2F> locations(samples.size());

        const float target_radius = 0.1;
        
        for(unsigned int i = 0; i < locations.size(); i++)
        {
            locations[i].x = samples[i].coordinate[1]*target_radius/radius;
            locations[i].y = samples[i].coordinate[0]*target_radius/radius;
        }

        // ensure consistent scaling
        for(unsigned int i = 0; i < region.size(); i++)
        {
            region[i] *= target_radius/radius;
        }
        
        if(! FrameBuffer::WriteFIG(locations, region, target_radius, (color_scheme%2 != 0), (color_scheme < 0), output_file_name))
        {
            cerr << "error in writing " << endl;
            return 1;
        }

        return 0;
    }
    else if(strstr(output_file_name, ".ppm"))
    {
        if(image_size <= 0)
        {
            cerr << "wrong image size" << endl;
            return 1;
        }
        
        cout << "snap point samples to output image" << endl;

        vector< vector<FrameBuffer::P3> > output_image(image_size);
        {
            for(unsigned int i = 0; i < output_image.size(); i++)
            {
                output_image[i] = vector<FrameBuffer::P3>(image_size);
            }
        }

        {
            for(int i = 0; i < image_size; i++)
                for(int j = 0; j < image_size; j++)
                {
                    output_image[i][j].r = output_image[i][j].g = output_image[i][j].b = 255*(color_scheme > 0);
                }
        }
    
        {
            for(unsigned int i = 0; i < samples.size(); i++)
            {
                int row = static_cast<int>(floor(samples[i].coordinate[0] * image_size));
                if(row >= image_size) row = image_size-1;
                int col = static_cast<int>(floor(samples[i].coordinate[1] * image_size));
                if(col >= image_size) col = image_size-1;

                output_image[row][col].r = output_image[row][col].g = output_image[row][col].b = 255*(color_scheme <= 0);
            }
        
            if(! FrameBuffer::WritePPM(output_image, 255, output_file_name))
            {
                throw Exception("error in writing file!");
            }
        }
    }
    else
    {
        cerr << "unknown output format" << endl;
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
