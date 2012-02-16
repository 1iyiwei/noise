/*
  SFT.cpp

  power fourier transform
  Li-Yi Wei
  
  08/24/2008: svn cp from BlueNoise/src/Main/SFT.cpp
              remove ppm output option for clarity
*/

#define _NO_DC_REMOVAL

#define _LEVERAGE_SYMMETRY

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
#include "LinearLFT.hpp"
#include "RandomLFT.hpp"
#include "DeformableLFT.hpp"

#include "IsotropicDeformer.hpp"

#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"
#include "Math.hpp"
#include "Utility.hpp"

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
    int min_argc = 7;

#if defined(_RANDOM_LFT)
    min_argc++;
#endif

#if defined(_ISOTROPIC_LFT)
    min_argc += 3;
#endif

#if defined(_BAND_LFT)
    min_argc += 2;
#endif

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name output-file-name dimension dimension-specs (*dimension* integers with positive ones indicate the 2D slices and the rest negative/zero ones indicate constant sampling frequency) dft-image-size dft-scale (i.e. maximum frequency in pixels. when dft-scale == dft-image-size no scaling occurs. use <= 0 for default built in value)";

#if defined(_RANDOM_LFT)
        cerr << " percentage";
#endif

#if defined(_ISOTROPIC_LFT)
        cerr << " global_mean distance_field_name min_distance (used during sampling) domain_size (dimension floats)";
#endif

#if defined(_BAND_LFT)
        cerr << " distance_lower_bound distance_upper_bound";
#endif

        cerr << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    vector<int> dimension_spec;
    if(argc < (min_argc-1+dimension))
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

    if((dft_scale_spec > 0) && (dft_scale_spec%fft_image_size != 0))
    {
#if 0
        cerr << "illegal dft-scale specification" << endl;
        return 1;
#else
        cerr << "warning: fractional dft_scale spec" << endl;
#endif
    }
#if 0
    if(fft_image_size%2)
    {
        cerr << "fft_image_size must be an even number" << endl;
        return 1;
    }
#endif

#if defined(_RANDOM_LFT)
    const float percentage = atof(argv[++argCtr]);
#endif

#if defined(_ISOTROPIC_LFT)
    const float global_mean = atof(argv[++argCtr]);
    const char * distance_field_name = argv[++argCtr];
    const float min_distance = atof(argv[++argCtr]);
    vector<float> domain_size(dimension);
    if((argc - 1 - argCtr) < dimension)
    {
        cerr << "insufficient arguments for domain_size" << endl;
        return 1;
    }
    else
    {
        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            domain_size[i] = atof(argv[++argCtr]);
        }
    }
#endif

#if defined(_BAND_LFT)
    if((argc - 1 - argCtr) < 2)
    {
        cerr << "insufficient arguments for distance bounds" << endl;
        return 1;
    }

    const float distance_lower_bound = atof(argv[++argCtr]);
    const float distance_upper_bound = atof(argv[++argCtr]);
#endif

    ifstream input(input_file_name);

    // read in samples
    cerr << "read in samples " << endl;
    
    vector<Sample> samples;
    
    if(!input)
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
            sample.id = 0;

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
   
#if defined(_LINEAR_LFT)
    LinearLFT pft(samples);
#elif defined(_RANDOM_LFT)
    RandomLFT pft(samples, percentage);
#elif defined(_ISOTROPIC_LFT)
    const DistanceField * distance_field = Utility::GetDistanceField(domain_size, vector<float>(1, min_distance), distance_field_name);
    IsotropicDeformer deformer(*distance_field, global_mean);
#if defined(_BAND_LFT)
    DeformableLFT pft(domain_size, samples, deformer, distance_lower_bound, distance_upper_bound);
#else
    DeformableLFT pft(domain_size, samples, deformer);
#endif
#elif defined(_DFT)
    DFT pft(samples);
#else
    SFT pft(samples);
#endif

    vector<int> fourier_magnitude_size(display_dimension);
    {
        for(int i = 0; i < display_dimension; i++)
        {
            fourier_magnitude_size[i] = fft_image_size;
        }
    }
    Array<float> fourier_magnitude(fourier_magnitude_size);
    fourier_magnitude.Clear(-1);

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
        float value;

        vector<int> digit_min(dimension);
        vector<int> digit_max(dimension);

        int first_positive_dimension_spec = -1;
        {
            for(int i = 0; i < dimension; i++)
            {
                if(dimension_spec[i] > 0)
                {
                    if(first_positive_dimension_spec < 0)
                    {
                        first_positive_dimension_spec = i;
                    }
#ifdef _LEVERAGE_SYMMETRY
                    digit_min[i] = 0; digit_max[i] = (first_positive_dimension_spec == i ? fft_image_size/2 : fft_image_size);
#else 
                    digit_min[i] = 0; digit_max[i] = fft_image_size-1;
#endif
                }
                else
                {
                    digit_min[i] = digit_max[i] = -dimension_spec[i];
                }
            }
        }
        
        if(first_positive_dimension_spec < 0)
        {
            throw Exception("first_positive_dimension_spec < 0");
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
            
            value = pft.Evaluate(w);

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
            
            const float final_value = (value >= 0? +1 : -1)*sqrt(fabs(value));

            int all_inside = 1;
            for(unsigned int i = 0; i < output_index.size(); i++)
            {
                all_inside *= ((output_index[i] >= 0) && (output_index[i] < fft_image_size));
            }
            if(all_inside)
            {
                if(!fourier_magnitude.Put(output_index, final_value))
                {
                    cerr << "cannot set output value" << endl;
                    return 1;
                }
            }

#ifdef _LEVERAGE_SYMMETRY
            // rotational symmetry component
            {
                int all_inside = 1;
                for(unsigned int i = 0; i < output_index.size(); i++)
                {
                    output_index[i] = fft_image_size/2*2 - output_index[i];
                    all_inside *= ((output_index[i] >= 0) && (output_index[i] < fft_image_size));
                }
                if(all_inside)
                {
                    if(!fourier_magnitude.Put(output_index, final_value))
                    {
                        cerr << "cannot set mirror output value" << endl;
                        return 1;
                    }
                }
            }
#endif
        }
        while(counter.Next());

#ifdef _NO_DC_REMOVAL
        int is_dc_slice = 0;
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
