/*
  PFM2PPM.cpp

  conversion from pfm to ppm format
  if pfm has dimension > 2, a chosen 2D slice is dumped out

  Li-Yi Wei
  07/09/2007

  08/16/2010
  handle power instead of magnitude

*/

#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>
using namespace std;

#include "Exception.hpp"
#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"
#include "OnionLayerCounter.hpp"

int CountPositive(vector<int> & numbers)
{
    int positive_count = 0;
    for(unsigned int i = 0; i < numbers.size(); i++)
    {
        positive_count += (numbers[i] > 0);
    }

    return positive_count;
}

int GetPositive(const vector<int> & dimension_spec, const vector<int> & input, vector<int> & output)
{
    if(dimension_spec.size() != input.size())
    {
        return 0;
    }
    
    output.clear();
    
    for(unsigned int i = 0; i < input.size(); i++)
    {
        if(dimension_spec[i] > 0)
        {
            output.push_back(input[i]);
        }
    }

    return 1;
}

int Map(const float input, const float input_max, const float input_min, const int output_max)
{
    // return static_cast<int>((input_max == input_min ? output_max/2: floor((min(fabs(input), input_max)-input_min)/(input_max-input_min)*output_max)));

    return static_cast<int>((input_max == input_min ? output_max/2: floor((min(input, input_max)-input_min)/(input_max-input_min)*output_max)));
}

enum XformOption {XFORM_NONE, XFORM_SQRT, XFORM_LOG};
enum ClampOption {CLAMP_NONE, CLAMP_DC, CLAMP_AXIS};

struct DisplayOption
{
    XformOption xform;
    ClampOption clamp;
};

bool Translate(const string & input, DisplayOption & output)
{ 
    XformOption xform;
    ClampOption clamp;

    // numerical inputs for backward compatibility
    if(input == "0")
    {
        xform = XFORM_SQRT;
        clamp = CLAMP_DC;
    }
    else if(input == "1")
    {
        xform = XFORM_LOG;
        clamp = CLAMP_DC;
    }
    else if(input == "2")
    {
        xform = XFORM_NONE;
        clamp = CLAMP_NONE;
    }
    else
    {
        // xform
        if(input.find("sqrt") != string::npos)
        {
            xform = XFORM_SQRT;
        }
        else if(input.find("log") != string::npos)
        {
            xform = XFORM_LOG;
        }
        else
        {
            xform = XFORM_NONE;
        }

        if(input.find("dc") != string::npos)
        {
            clamp = CLAMP_DC;
        }
        else if(input.find("axis") != string::npos)
        {
            clamp = CLAMP_AXIS;
        }
        else
        {
            clamp = CLAMP_NONE;
        }
    }

    output.xform = xform;
    output.clamp = clamp;

    return true;
}

int Mismatch(const vector<int> & dc_index, const vector<int> & query_index)
{
    int answer = 0;

    if(dc_index.size() != query_index.size())
    {
        throw Exception("IsAxis(): dimension mismatch");
    }

    for(unsigned int i = 0; i < dc_index.size(); i++)
    {
        answer += (query_index[i] != dc_index[i]);
    }

    return answer;
}

float FindMax(const bool eligible, const float current_max, const float current_value)
{
    float new_max = current_max;

    if(eligible && (current_value > new_max))
    {
        new_max = current_value;
    }

    return new_max;
}

int Main(int argc, char **argv)
{
    int num_fixed_arguments = 4;

#ifdef _CLAMP_RADIUS
    num_fixed_arguments += 1;
#endif

    if(argc < (num_fixed_arguments+2))
    {
        cerr << "Usage: " << argv[0] << " input-file-name output-file-name dimension-specs (dimension-number of integers with exactly two positive ones indicate the 2D slices and the rest negative/zero ones indicate constant sampling frequency) display_option (0 for magnitude/default, 1 for log(magnitude^2), 2 for power/no-processing (i.e. not assuming it is a fft power spectrum))";

#ifdef _CLAMP_RADIUS
        cerr << " clamp_radius (effective for display_option 0 and 1)";
#endif

        cerr << " [optional: maximum_value]" << endl;
        return 1;
    }
    
    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const char * output_file_name = argv[++argCtr];
    vector<int> dimension_spec;

    Array<FrameBuffer::PF> input_spectrum;

    if(! FrameBuffer::ReadPFM(input_file_name, input_spectrum))
    {
        cerr << "error in reading " << input_file_name << endl;
        return 1;
    }

    const int dimension = input_spectrum.Dimension();

    if(dimension < 2)
    {
        cerr << "input dimension must be at least 2" << endl;
        return 1;
    }

    if(argc < (num_fixed_arguments+dimension))
    {
        cerr << "wrong number of input arguments" << endl;
        return 1;
    }
    else
    { 
        for(int i = 0; i < dimension; i++)
        {
            dimension_spec.push_back(atoi(argv[++argCtr]));
        }
    }

    const string display_option_string = argv[++argCtr];
    DisplayOption display_option;
    if(!Translate(display_option_string, display_option))
    {
        cerr << "cannot translate display option" << endl;
        return 1;
    }

    const XformOption xform_option = display_option.xform;
    const ClampOption clamp_option = display_option.clamp;

#ifdef _CLAMP_RADIUS
    const float clamp_radius = atof(argv[++argCtr]);
#else
    const float clamp_radius = 0;
#endif

    const int display_dimension = CountPositive(dimension_spec);
    
    if(display_dimension != 2)
    {
        cerr << "dimension spec must have exactly 2 positive numbers" << endl;
        return 1;
    }

    const float maximum_value = (argc > (num_fixed_arguments+dimension) ? atof(argv[++argCtr]) : -1);

    vector<int> input_size;
    input_spectrum.Size(input_size);

    if(input_size.size() != dimension_spec.size())
    {   
        cerr << "input_size.size() != dimension_spec.size()" << endl;
        return 1;
    }
    else
    {
        for(unsigned int i = 0; i < dimension_spec.size(); i++)
        {
            if((dimension_spec[i] <= 0) && (-dimension_spec[i] >= input_size[i]))
            {
                cerr << "dimension spec out of range" << endl;
                return 1;
            }
        }
    }
    
    vector<int> output_size;
    if(! GetPositive(dimension_spec, input_size, output_size))
    {
        cerr << "cannot GetPositive() " << endl;
        return 1;
    }

    const int output_max = 255;

    Array<FrameBuffer::P3> output_spectrum(output_size);

    vector<int> digit_min(dimension);
    vector<int> digit_max(dimension);
    {
        for(int i = 0; i < dimension; i++)
        {
            if(dimension_spec[i] > 0)
            {
                digit_min[i] = 0; digit_max[i] = input_size[i]-1;
            }
            else
            {
                digit_min[i] = digit_max[i] = -dimension_spec[i];
            }
        }
    }

    // dc stuff
    int is_dc_slice = 1;
    {
        for(unsigned int i = 0; i < dimension_spec.size(); i++)
        {
            if((dimension_spec[i] <= 0) && (-dimension_spec[i] != input_size[i]/2))
            {
                is_dc_slice = 0;
            }
        }
    }  

    vector<int> dc_index = input_size;
    for(unsigned int i = 0; i < dc_index.size(); i++)
    {   
        dc_index[i] = input_size[i]/2;
    }

    // color space transformation
    {
        if(xform_option == XFORM_SQRT)
        {
            // magnitude
            vector<FrameBuffer::PF> input_pixels;
            input_spectrum.Get(input_pixels);

            for(unsigned int i = 0; i < input_pixels.size(); i++)
            {
                FrameBuffer::PF sqrted;

                // careful coding as r/g/b might be a union sharing memory
                sqrted.r = sqrt(fabs(input_pixels[i].r));
                sqrted.g = sqrt(fabs(input_pixels[i].g));
                sqrted.b = sqrt(fabs(input_pixels[i].b));

                // potential negative values
                sqrted.r *= (input_pixels[i].r >= 0? +1 : -1);
                sqrted.g *= (input_pixels[i].g >= 0? +1 : -1);
                sqrted.b *= (input_pixels[i].b >= 0? +1 : -1);

                input_pixels[i] = sqrted;
            }
            
            input_spectrum.Put(input_pixels);
        } 
        else if(xform_option == XFORM_LOG)
        {
            // log(magnitude^2)

            vector<FrameBuffer::PF> input_pixels;
            input_spectrum.Get(input_pixels);

            for(unsigned int i = 0; i < input_pixels.size(); i++)
            {
                FrameBuffer::PF logged;

                // careful coding as r/g/b might be a union sharing memory
                logged.r = log(input_pixels[i].r > 0 ? input_pixels[i].r : 1.0);
                logged.g = log(input_pixels[i].g > 0 ? input_pixels[i].g : 1.0);
                logged.b = log(input_pixels[i].b > 0 ? input_pixels[i].b : 1.0);
#if 0                
                // potential negative values
                logged.r *= (input_pixels[i].r >= 0? +1 : -1);
                logged.g *= (input_pixels[i].g >= 0? +1 : -1);
                logged.b *= (input_pixels[i].b >= 0? +1 : -1);
#endif           
                input_pixels[i] = logged;
            }
            
            input_spectrum.Put(input_pixels);
        }
        else
        {
            // pass through
        }
    }
    
    // find input max min
    FrameBuffer::PF input_max, input_min, input_mean;
    input_max.r = input_max.g = input_max.b = -1.0/sin(0.0);
    input_min.r = input_min.g = input_min.b =  1.0/sin(0.0);
    input_mean.r = input_mean.g = input_mean.b = 0;
 
    SequentialCounter counter(dimension, digit_min, digit_max);
    counter.Reset();
    vector<int> input_index(dimension);

    {
        FrameBuffer::PF pixel;

        do
        {
            counter.Get(input_index);

            if(! input_spectrum.Get(input_index, pixel))
            {
                cerr << "error in getting input pixel" << endl;
                return 1;
            }

#ifdef _CLAMP_RADIUS
            float radius2 = 0;
            for(unsigned int i = 0; i < input_index.size(); i++)
            {
                radius2 += (input_index[i]-dc_index[i])*(input_index[i]-dc_index[i]);
            }

            const bool eligible = (radius2 > clamp_radius*clamp_radius);
#else
            const int mismatch = Mismatch(dc_index, input_index);
            const bool eligible = (clamp_option == CLAMP_AXIS? (mismatch > 1) : (mismatch > 0));
#endif
           
            const bool max_eligible = ((clamp_option == CLAMP_NONE) || eligible);

            input_max.r = FindMax(max_eligible, input_max.r, pixel.r);
            input_max.g = FindMax(max_eligible, input_max.g, pixel.g);
            input_max.b = FindMax(max_eligible, input_max.b, pixel.b);

            if(pixel.r < input_min.r) input_min.r = pixel.r;
            if(pixel.g < input_min.g) input_min.g = pixel.g;
            if(pixel.b < input_min.b) input_min.b = pixel.b;

            input_mean.r += pixel.r;
            input_mean.g += pixel.g;
            input_mean.b += pixel.b;
        }
        while(counter.Next());
    }

    // clamp values
    {
        vector<FrameBuffer::PF> input_pixels;
        input_spectrum.Get(input_pixels);
        
        if(maximum_value >= 0)
        {
            input_max.r = input_max.g = input_max.b = maximum_value;
            input_min.r = input_min.g = input_min.b = 0.0;
            
            cerr << "changed - ";
        }
        else
        {
        }
                
        cerr << "max: " << input_max.g << ", min: " << input_min.g << ", mean: " << input_mean.r/input_pixels.size() << " " << input_mean.g/input_pixels.size() << " " << input_mean.b/input_pixels.size() << endl;

        for(unsigned int i = 0; i < input_pixels.size(); i++)
        {
            FrameBuffer::PF & pixel = input_pixels[i];
            
            if(pixel.r > input_max.r) pixel.r = input_max.r;
            if(pixel.g > input_max.g) pixel.g = input_max.g;
            if(pixel.b > input_max.b) pixel.b = input_max.b;
        }
        
        input_spectrum.Put(input_pixels);
    }
    
    // fill output
    counter.Reset();

    vector<int> output_index;
    FrameBuffer::PF input_pixel;
    FrameBuffer::P3 output_pixel;

    if(xform_option == XFORM_SQRT)
    {
        // make sure input_min >= 0
        if(input_min.r < 0) input_min.r = 0;
        if(input_min.g < 0) input_min.g = 0;
        if(input_min.b < 0) input_min.b = 0;
    }

    // find input maximum first
    do
    {
        counter.Get(input_index);
        GetPositive(dimension_spec, input_index, output_index);

        if(! input_spectrum.Get(input_index, input_pixel))
        {
            cerr << "error in getting input pixel" << endl;
            return 1;
        }

        if(xform_option == XFORM_SQRT)
        {
            // show negative values as red
            output_pixel.r = Map(input_pixel.r, input_max.r, input_min.r, output_max);
            output_pixel.g = (input_pixel.g > 0)*Map(input_pixel.g, input_max.g, input_min.g, output_max);
            output_pixel.b = (input_pixel.b > 0)*Map(input_pixel.b, input_max.b, input_min.b, output_max);
        }
        else if(xform_option == XFORM_LOG)
        {
            output_pixel.r = Map(fabs(input_pixel.r), input_max.r, input_min.r, output_max); 
            output_pixel.g = Map(fabs(input_pixel.g), input_max.g, input_min.g, output_max); 
            output_pixel.b = Map(fabs(input_pixel.b), input_max.b, input_min.b, output_max);
        }
        else
        {
            output_pixel.r = Map(input_pixel.r, input_max.r, input_min.r, output_max); 
            output_pixel.g = Map(input_pixel.g, input_max.g, input_min.g, output_max); 
            output_pixel.b = Map(input_pixel.b, input_max.b, input_min.b, output_max);
        }

        if(! output_spectrum.Put(output_index, output_pixel))
        {
            cerr << "error in putting output pixel" << endl;
            return 1;
        }
    }
    while(counter.Next());

    // dump out
    if(! FrameBuffer::WritePPM(output_spectrum, output_max, output_file_name))
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
