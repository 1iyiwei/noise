/*
  SamplePFT.cpp

  power fourier transform
  Li-Yi Wei
  
  08/24/2008: 
  svn cp from BlueNoise/src/Main/SFT.cpp
  remove ppm output option for clarity

  08/16/2010: 
  migration from Rainbow, and remove old options
  output true power (squared magnitude) instead of magnitude  

  08/30/2010:
  rename to SamplePFT.cpp
  cleanup synthetic option via UniformAFT class
*/

#define _NO_DC_REMOVAL

#define _LEVERAGE_SYMMETRY

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <float.h>
//#include <limits.h>

#include "Exception.hpp"

#include "SFT.hpp"

#include "AFT.hpp"
#include "UniformAFT.hpp"
#include "AdaptiveAFT.hpp"

#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"
#include "Math.hpp"
#include "Utility.hpp"
#include "Timer.hpp"

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

int ParseDomainSize(const string & input_in, vector<float> & output)
{
    string input(input_in);

    // replace all non-numerical and non . into space
    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(((input[i] < '0') || (input[i] > '9')) && (input[i] != '.'))
        {
            input[i] = ' ';
        }
    }

    stringstream ss(input);

    output.clear();
    float value;
    while(ss >> value)
    {
        output.push_back(value);
    }

    return 1;
}

int Main(int argc, char **argv)
{
    int min_argc = 8;

#ifdef _SYNTHETIC_PFT
    min_argc += 2;
#endif

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0];

#ifndef _SYNTHETIC_PFT
        cerr << " samples-file-name";
#else
        cerr << " num_samples domain_option (uniform, f2m file for adaptive) domain_size_string (e.g. 1x1 for 2D unit domain)";
#endif

        cerr << " output-file-name dimension dimension-specs (*dimension* integers with positive ones indicate the 2D slices and the rest negative/zero ones indicate constant sampling frequency) dft-image-size dft-scale (i.e. maximum frequency in pixels. when dft-scale == dft-image-size no scaling occurs. use <= 0 for default built in value) normalization-option (sqrtsamplecount for align at freq-infinity, samplecount for align at freq-DC)";

        cerr << endl;
        return 1;
    }

#ifdef _SYNTHETIC_PFT
    const string uniform_synthesis = "uniform";
#endif

    int argCtr = 0;
#ifndef _SYNTHETIC_PFT
    const string input_file_name = argv[++argCtr];
#else
    const int num_samples = atoi(argv[++argCtr]);
    const string synthesis_option = argv[++argCtr];
    const string domain_size_string = argv[++argCtr];
#endif
    const string output_file_name = argv[++argCtr];
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
    const string normalization_option = argv[++argCtr];

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

#ifdef _SYNTHETIC_PFT
    cerr << "dummy samples " << endl;
    vector<Sample> samples(num_samples);
#else
    // read in samples
    cerr << "read in samples " << endl;
    
    vector<Sample> samples;

    const Utility::SampleMode input_mode = (((input_file_name.find(".txt") == string::npos) && (input_file_name != "cin")) ? Utility::SAMPLE_BINARY : Utility::SAMPLE_ASCII);

    if(! Utility::ReadSamples(input_mode, 0, dimension, input_file_name, samples))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }
#endif // #ifdef _SYNTHETIC_PFT

    float normalization_const = 1.0;

    if(normalization_option == "sqrtsamplecount")
    {
        normalization_const = 1.0/sqrt(1.0*samples.size());
    }
    else if(normalization_option == "samplecount")
    {
        normalization_const = 1.0/samples.size();
    }
    else
    {
        cerr << "unknown normalization option" << endl;
        return 1;
    }

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        samples[i].value = normalization_const;
    }
    
    cerr << "SamplePFT: total " << samples.size() << " samples read" << endl;
    
    // compute sample fourier transform
    cerr << "compute sample fourier transform" << endl;
   
#ifdef _SYNTHETIC_PFT
    const AFT * p_aft = 0;

    if(synthesis_option == uniform_synthesis)
    {
        p_aft = new UniformAFT(samples.size(), normalization_const);
    }
    else if(synthesis_option.find(".f2m") != string::npos)
    {
        vector<float> domain_size;
        if(! ParseDomainSize(domain_size_string, domain_size))
        {
            cerr << "cannot parse domain size" << endl;
            return 1;
        }

        if(domain_size.size() != dimension)
        {
            cerr << "incorrect domain_size dimension: " << domain_size.size() << " != " << dimension << endl;
            return 1;
        }

        Array<float> importance;
        if(! FrameBuffer::ReadF2M(synthesis_option, importance))
        {
            cerr << "cannot read from " << synthesis_option << endl;
            return 1;
        }

        if(importance.Dimension() != dimension)
        {
            cerr << "incorrect importance dimension" << endl;
            return 1;
        }

        // normalize
        const string message = Utility::Normalize(importance, domain_size);
        if(message != "")
        {
            cerr << "error in normalizing importance: " << message << endl;
            return 1;
        } 

        p_aft = new AdaptiveAFT(samples.size(), normalization_const, domain_size, importance);
    }
    else
    {
        cerr << "unknown synthesis option" << endl;
        return 1;
    }

    if(!p_aft) throw Exception("null p_aft");

    const PFT & pft = *p_aft;
#else
#if defined(_DFT)
    DFT pft(samples);
#else
    SFT pft(samples);
#endif
#endif // #ifdef _SYNTHETIC_PFT

    vector<int> fourier_power_size(display_dimension);
    {
        for(int i = 0; i < display_dimension; i++)
        {
            fourier_power_size[i] = fft_image_size;
        }
    }
    Array<float> fourier_power(fourier_power_size);
    fourier_power.Clear(-1);

    Timer timer;

    double compute_time = 0;

    if(!timer.Start()) throw Exception("cannot start timer");

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

            if(0)
            {
                // debug
                int is_dc = 1;
                for(int k = 0; k < w.Dimension(); k++)
                {
                    is_dc *= (w[k] == 0);
                }

                if(is_dc)
                {
                    cerr << "dc value: " << value << endl;
                }
            }

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
            
            const float final_value = value; // (value >= 0? +1 : -1)*sqrt(fabs(value)); // output power instead of magnitude

            int all_inside = 1;
            for(unsigned int i = 0; i < output_index.size(); i++)
            {
                all_inside *= ((output_index[i] >= 0) && (output_index[i] < fft_image_size));
            }
            if(all_inside)
            {
                if(!fourier_power.Put(output_index, final_value))
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
                    if(!fourier_power.Put(output_index, final_value))
                    {
                        cerr << "cannot set mirror output value" << endl;
                        return 1;
                    }
                }
            }
#endif
        }
        while(counter.Next());

        int is_dc_slice = 1;
        
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

#ifndef _NO_DC_REMOVAL
            // cerr << "zero out " << i << ", " << j << endl;
            fourier_power.Put(output_index, 0); // no DC component
#endif
        }
    }

    if(!timer.Stop()) throw Exception("cannot stop timer");

    compute_time += timer.ElapsedTime();

    // timing info
    cout << "compute time: " << compute_time << endl;

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
        if(! fourier_power.Get(input_pixels))
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

    // clean up
#ifdef _SYNTHETIC_PFT
    if(! p_aft)
    {
        delete p_aft;
        p_aft = 0;
    }
#endif

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
