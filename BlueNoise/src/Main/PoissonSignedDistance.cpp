/*
  PoissonSignedDistance.cpp

  use red and green colors to visualize the signed distance function
  of an underlying Poisson sphere distributions

  Li-Yi Wei
  06/29/2007

*/


#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "Exception.hpp"
#include "FrameBuffer.hpp"

class Coordinate
{
public:
    Coordinate(void)
        {
        }
    
    Coordinate(const int dim) : _values(dim)
        {
            for(unsigned int i = 0; i < _values.size(); i++)
            {
                _values[i] = -1;
            }
        }

    int Dimension(void) const
        {
            return _values.size();
        }
    
    float & operator[](const int index)
        {
            assert((index >= 0) && (static_cast<unsigned int>(index) < _values.size()));
            return _values[index];
        }
    
    float operator[](const int index) const
        {
            assert((index >= 0) && (static_cast<unsigned int>(index) < _values.size()));
            return _values[index];
        }
    
protected:
    vector<float> _values;
};

// normal values should be in the range [-1, +inf]
float ComputeColor(const Coordinate & center, const float radius, const Coordinate & query)
{
    if(center.Dimension() != query.Dimension())
    {
        return -2;
    }

    if(radius <= 0)
    {
        return -3;
    }
    
    float distance = 0;
    for(int i = 0; i < center.Dimension(); i++)
    {
        float foo = center[i]-query[i];
        distance += foo*foo;
    }
    distance = sqrt(distance/center.Dimension());

    return (distance/radius - 1);
}

int Main(int argc, char **argv)
{
    if(argc < 8)
    {
        cerr << "Usage: " << argv[0] << " dimension radius samples-file-name output-file-name image-size view (x=0, y=1, z=2) view_value (x/y/z slice in [0 image-size-1])" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float radius = atof(argv[++argCtr]);
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int image_size = atoi(argv[++argCtr]);
    const int view = (dimension == 2 ? 2 : atoi(argv[++argCtr]));
    const int view_value = (dimension == 2 ? 0 : atoi(argv[++argCtr]));

    if((dimension < 2) || (dimension > 3))
    {
        cerr << "dimension must be 2 or 3" << endl;
        return 1;
    }

    if((dimension == 3) && ((view < 0) || (view > 2)))
    {
        cerr << "view must be 0 or 1 or 2" << endl;
        return 1;
    }

    if((dimension == 3) && ((view_value < 0) || (view_value >= image_size)))
    {
        cerr << "view_value must be within [0 image_size-1]" << endl;
        return 1;
    }
    
    ifstream input(input_file_name); // read in samples
    cout << "read in samples " << endl;
    
    vector<Coordinate> samples;
    
    if(!input)
    {
        return 0;
    }
    else
    {
        Coordinate sample(dimension);
        
        while(input.good())
        {
            for(unsigned int i = 0; i < sample.Dimension(); i++)
            {
                input >> sample[i];
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    cout << "total " << samples.size() << " samples read" << endl;

    // populate samples to output float buffer
    cout << "populate samples to output float buffer" << endl;

    vector< vector<float> > output_buffer(image_size);
    {
        for(unsigned int i = 0; i < output_buffer.size(); i++)
        {
            output_buffer[i] = vector<float>(image_size);

            for(unsigned int j = 0; j < output_buffer[i].size(); j++)
            {
                output_buffer[i][j] = 1000000.0; // FLT_MAX
            }
        }
    }
    
    {
        const int radius_in_pixels = static_cast<int>(ceil(3*radius*image_size))+1;
        
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            // add the impact for each sample
            const Coordinate & center = samples[i];

            // note the following code works for both 2D and 3D!
            const int center_row = static_cast<int>(floor(center[(view+1)%dimension]*image_size));
            const int center_col = static_cast<int>(floor(center[(view+2)%dimension]*image_size));

            for(int row_offset = -radius_in_pixels;
                row_offset <= radius_in_pixels; row_offset++)
                for(int col_offset = -radius_in_pixels;
                    col_offset <= radius_in_pixels; col_offset++)
                {
                    const int row = center_row + row_offset;
                    const int col = center_col + col_offset;

                    if((row >= 0) && (row < image_size) &&
                       (col >= 0) && (col < image_size))
                    {
                        Coordinate query(dimension);

                        query[(view)%dimension] = (view_value+0.5)/image_size;
                        query[(view+1)%dimension] = (row+0.5)/image_size;
                        query[(view+2)%dimension] = (col+0.5)/image_size;

                        const float new_value = ComputeColor(center, radius, query);
                        if(output_buffer[row][col] > new_value)
                        {
                            output_buffer[row][col] = new_value;
                        }
                    }
                }
        }
    }
    
    // transform output buffer to output image
    cout << "transform output buffer to output image" << endl;

    vector< vector<FrameBuffer::P3> > output_image(image_size);
    {
        for(unsigned int i = 0; i < output_image.size(); i++)
        {
            output_image[i] = vector<FrameBuffer::P3>(image_size);
        }
    }

    {
        // do transformation here
        for(int i = 0; i < image_size; i++)
            for(int j = 0; j < image_size; j++)
            {
                if(output_buffer[i][j] < 0)
                {
                    output_image[i][j].r = static_cast<int>(floor(output_buffer[i][j]*-255));
                    if(output_image[i][j].r > 255) output_image[i][j].r = 255;
                    output_image[i][j].g = output_image[i][j].b = 0;
                }
                else
                {
                    output_image[i][j].g = static_cast<int>(floor(output_buffer[i][j]*255));
                    if(output_image[i][j].g > 255) output_image[i][j].g = 255;
                    output_image[i][j].r = output_image[i][j].b = 0;
                }
            }
        
        // write out
        if(! FrameBuffer::WritePPM(output_image, 255, output_file_name))
        {
            throw Exception("error in writing file!");
        }
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
