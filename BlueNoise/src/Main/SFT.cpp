/*
  SFT.cpp

  Li-Yi Wei
  
  06/15/2007 : 2D 
  07/08/2008 : arbitrary D

  TODO:
  for high dimensions with more than 2 positive dimension specs I should dump out the pfm file instead of enforcing on 2D slices. This will make the program more orthogonal to each other. Also need to change RadialPlot to handle arbitrary dimensions. I should do this by making my pfm format handle arbitrary dimensions.
*/

#define _NO_DC_REMOVAL

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <float.h>
//#include <limits.h>

#include "Exception.hpp"
#include "SFT.hpp"
#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"
#include "Math.hpp"

const double PI = Math::PI;

int CountPositive(vector<int> & numbers)
{
    int positive_count = 0;
    for(unsigned int i = 0; i < numbers.size(); i++)
    {
        positive_count += (numbers[i] > 0);
    }

    return positive_count;
}

int Main(int argc, char **argv)
{
    if(argc < 7)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name output-file-name dimension dimension-specs (*dimension* integers with positive ones indicate the 2D slices and the rest negative/zero ones indicate constant sampling frequency) dft-image-size dft-scale (i.e. maximum frequency in pixels. when dft-scale == dft-image-size no scaling occurs. use <= 0 for default built in value)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    vector<int> dimension_spec;
    if(argc < (6+dimension))
    {
        cerr << "insufficient number of arguments" << endl;
        return 1;
    }
    else
    {
        for(int i = 0; i < dimension; i++)
        {
            dimension_spec.push_back(atoi(argv[++argCtr]));
        }
    }

    const int display_dimension = CountPositive(dimension_spec);
    
#if 0
    if(display_dimension < 2)
    {
        cerr << "dimension spec must have at least 2 positive numbers" << endl;
        return 1;
    }
#endif

    const int fft_image_size = atoi(argv[++argCtr]);
    const int dft_scale_spec = atoi(argv[++argCtr]); 
    const int max_value = -1; // (argc > 5? atof(argv[5]) : -1);

    int ppm_or_pfm = -1;

    if(strstr(output_file_name, ".ppm"))
    {
        if(display_dimension == 2)
        {
            cerr << "output is ppm " << endl;
            ppm_or_pfm = 0;
        }
        else
        {
            cerr << "output is pfm " << endl;
            ppm_or_pfm = 1;
        }
    }
    else if(strstr(output_file_name, ".pfm"))
    {
        cerr << "output is pfm " << endl;
        ppm_or_pfm = 1;
    }
    else
    {
        cerr << "output file must be ppm or pfm" << endl;
        return 1;
    }

    if((dft_scale_spec > 0) && (dft_scale_spec%fft_image_size != 0))
    {
        cerr << "illegal dft-scale specification" << endl;
        return 1;
    }
#if 0
    if(fft_image_size%2)
    {
        cerr << "fft_image_size must be an even number" << endl;
        return 1;
    }
#endif
    ifstream input(input_file_name);

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
                input >> sample.coordinate[i];
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

#ifdef _NO_DC_REMOVAL
#ifndef _SFT_READ_VALUE
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        samples[i].value = 1.0/samples.size();
    }
#endif
#endif
    
    cerr << "total " << samples.size() << " samples read" << endl;
    
    // compute sample fourier transform
    cerr << "compute sample fourier transform" << endl;
    
    SFT sft(samples);

    vector<int> fourier_magnitude_size(display_dimension);
    {
        for(int i = 0; i < display_dimension; i++)
        {
            fourier_magnitude_size[i] = fft_image_size;
        }
    }
    Array<float> fourier_magnitude(fourier_magnitude_size);

    // scaling based on r_max as in Ares Lagae's thesis
    // multiply by 2 since my r_max is the minimum distance (twice the sphere radius)
    const float r_max = Math::ComputeMaxMinDistance(dimension, samples.size());

    if((r_max <= 0) && (dft_scale_spec <= 0))
    {
        cerr << "you have to specify dft_scale_spec for dimension " << dimension << endl;
        return 1;
    }
    
    {
        // don't know why, but if dft_scale is not an integer multiple of fft_image_size I will get spuriously large values around DC
        // probably this is the nature of DFT that we can only evaluate at interger frequencies? Li-Yi 07/08/2007
        const float dft_scale = (dft_scale_spec > 0 ? dft_scale_spec : ceil((8.0/r_max)/fft_image_size)*fft_image_size);

        cerr << "dft_scale " << dft_scale << ", r_max " << r_max << endl;
        
        Coordinate w(dimension);
        SFT::Value value;

        vector<int> digit_min(dimension);
        vector<int> digit_max(dimension);

        {
            for(int i = 0; i < dimension; i++)
            {
                if(dimension_spec[i] > 0)
                {
                    digit_min[i] = 0; digit_max[i] = fft_image_size-1;
                }
                else
                {
                    digit_min[i] = digit_max[i] = -dimension_spec[i];
                }
            }
        }
     
        SequentialCounter counter(dimension, digit_min, digit_max);

        counter.Reset();

	do
        {
            vector<int> w_index(dimension);
            vector<int> output_index(display_dimension);
            
            counter.Get(w_index);

            {
                for(int k = 0; k < w.Dimension(); k++)
                {
                    w[k] = static_cast<float>(2*PI*(w_index[k]-fft_image_size/2)*dft_scale/fft_image_size);
                }
            }
            
            value = sft.Evaluate(w);

            {
                int i = 0;
                for(int k = 0; k < dimension; k++)
                {
                    if(dimension_spec[k] > 0)
                    {
                        assert(i < display_dimension);
                        output_index[i] = w_index[k];
                        i++;
                    }
                }
            }
            
            if(!fourier_magnitude.Put(output_index, sqrt(value.r*value.r + value.i*value.i)))
            {
                cerr << "cannot set output value" << endl;
                return 1;
            }
        }
        while(counter.Next());

#ifdef _NO_DC_REMOVAL
        int is_dc_slice = (ppm_or_pfm == 0);
#else
        int is_dc_slice = 1;
#endif
        
        {
            for(unsigned int i = 0; i < dimension_spec.size(); i++)
            {
                if((dimension_spec[i] <= 0) && (-dimension_spec[i] != fft_image_size/2))
                {
                    is_dc_slice = 0;
                }
            }
        }
        
        if(is_dc_slice)
        {
            vector<int> output_index(display_dimension);
            {
                for(int k = 0; k < display_dimension; k++)
                {
                    output_index[k] = fft_image_size/2;
                }
            }
            // cerr << "zero out " << i << ", " << j << endl;
            fourier_magnitude.Put(output_index, 0); // no DC component
        }
    }

    // output stuff
    cerr << "output stuff " << endl;

    if(ppm_or_pfm == 1)
    {
        // pfm
        vector<int> output_size(display_dimension);
        {
            for(int i = 0; i < dimension; i++)
            {
                output_size[i] = fft_image_size;
            }
        }
               
        Array<FrameBuffer::PF> output(output_size);

        vector<float> input_pixels;
        if(! fourier_magnitude.Get(input_pixels))
        {
            cerr << "cannot get input pixels" << endl;
            return 1;
        }

        vector<FrameBuffer::PF> output_pixels;
        if(! output.Get(output_pixels))
        {
            cerr << "cannot get output pixels" << endl;
            return 1;
        }

        if(input_pixels.size() != output_pixels.size())
        {
            cerr << "input_pixels.size() != output_pixels.size()" << endl;
            return 1;
        }
        {
            for(unsigned int i = 0; i < input_pixels.size(); i++)
            {
                output_pixels[i].r = output_pixels[i].g = output_pixels[i].b = input_pixels[i];
            }
        }
        if(! output.Put(output_pixels))
        {
            cerr << "cannot put output pixels" << endl;
            return 1;
        }

        if(! FrameBuffer::WritePFM(output, output_file_name))
        {
            throw Exception("error in writing file!");
        }
    }
    else
    {
        // ppm
        assert(display_dimension == 2);
        
        vector< vector<FrameBuffer::P3> > output(fft_image_size);

        {
            for(unsigned int i = 0; i < output.size(); i++)
            {
                output[i] = vector<FrameBuffer::P3>(fft_image_size);
            }
        }
            
        // normalize first
        float maximum = 0;

        {
            for(unsigned int i = 0; i < fourier_magnitude.Size(0); i++)
                for(unsigned int j = 0; j < fourier_magnitude.Size(1); j++)
                {
                    vector<int> coord(display_dimension);
                    coord[0] = i; coord[1] = j;
                    float input_pixel;
                    if(! fourier_magnitude.Get(coord, input_pixel))
                    {
                        cerr << "error in getting input pixel" << endl;
                        return 1;
                    }
                    
                    if(input_pixel > maximum)
                    {
                        maximum = input_pixel;
                    }
                }

            cerr << "maximum " << maximum << endl;
            
            if((max_value > 0) && (max_value > maximum))
            {
                maximum = max_value;
                cerr << "maximum replaced by " << maximum << endl;
            }
            
            if(maximum > 0)
            {
                for(unsigned int i = 0; i < output.size(); i++)
                    for(unsigned int j = 0; j < output.size(); j++)
                    {
                        vector<int> coord(display_dimension);
                        coord[0] = i; coord[1] = j;
                        float input_pixel;
                        if(! fourier_magnitude.Get(coord, input_pixel))
                        {
                            cerr << "error in getting input pixel" << endl;
                            return 1;
                        }
                        
                        output[i][j].r = output[i][j].g = output[i][j].b = static_cast<int>(floor(input_pixel/maximum * 255));
                    }
            }
        }

        if(! FrameBuffer::WritePPM(output, 255, output_file_name))
        {
            throw Exception("error in writing file!");
        }
    }
    
    // throw Exception("not yet done!");

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
