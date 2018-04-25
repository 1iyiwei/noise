/*
  Utility.hpp

  Li-Yi Wei
  08/26/2008
  
*/

#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <string>
#include <vector>
using namespace std;

#include "Sample.hpp"
#include "Array.hpp"
#include "Matrix.hpp"
#include "SequentialCounter.hpp"
#include "FrameBuffer.hpp"

class Utility
{
public:
    
    // convert command line arguments to string list
    static vector<string> Convert(int argc, char *argv[]);

    // Sample stuff
    static Coordinate Diff(const Coordinate & c1, const Coordinate & c2);
    static Coordinate Add(const Coordinate & c1, const Coordinate & c2);
    static float Dot(const Coordinate & c1, const Coordinate & c2);
    static Coordinate Multiply(const Coordinate & c1, const CoordinateType & c);
    static float Distance2(const Coordinate & c1, const Coordinate & c2);
    static float Norm2(const Coordinate & c);
    static float Distancep(const CoordinateType & p, const Coordinate & c1, const Coordinate & c2);
    static float DistanceINF(const Coordinate & c1, const Coordinate & c2);
    static vector<int> Minus1(const vector<int> & input);

    // decide whether a sample is inside a domain
    static bool Inside(const vector<float> & domain, const Sample & sample);
    static bool Inside(const vector<float> & domain_min, const vector<float> & domain_max, const Sample & sample);

    // sample IO
    static int ReadSamples(const int dimension, const string & input_file_name, vector<Sample> & samples);

    static int WriteSamples(const vector<Sample> & samples, const string & output_file_name);

    static int ReadSamples(const int has_id, const int dimension, const string & input_file_name, vector<Sample> & samples);

    enum SampleMode {SAMPLE_ASCII, SAMPLE_BINARY};

    static int ReadSamples(const SampleMode & mode, const int has_id, const int dimension,const string & input_file_name, vector<Sample> & samples);

    static int WriteSamples(const SampleMode & mode, const int has_id, const vector<Sample> & samples, const string & output_file_name);

    // accounting
    static int CountSamples(const vector<Sample> & samples, vector<int> & num_samples_per_class);

    // drawing
    static string BuildPalette(const int num_colors, const string color_palette_file_name, vector<FrameBuffer::P3> & palette);

    // conversion
    template<class T>
    static void Convert(const CoordinateT<T> & input, vector<T> & output);

    template<class T>
    static int Convert(const Array<T> & input, const int element_dimension, Array< Array<T> > & output);
    
    template<class T>
    static int Convert(const Array< Array<T> > & input, Array<T> & output);

    // normalize a pdf/importance field so that the sum is 1.0
    // return empty string if successful, error message else
    static string Normalize(Array<float> & importance, const vector<float> & domain_size);

    static int ReadMatrixArray(const string & input_file_name, Array<Matrix> & matrix_array);

    static int WriteMatrixArray(const Array<Matrix> & matrix_array, const string & output_file_name);
};

template<class T>
void Utility::Convert(const CoordinateT<T> & input, vector<T> & output)
{
    output = vector<T>(input.Dimension());

    for(unsigned int i = 0; i < output.size(); i++)
    {
        output[i] = input[i];
    }
};

template<class T>
bool operator==(const vector<T> & v1, const vector<T> & v2)
{
    if(v1.size() != v2.size()) return 0;

    for(unsigned int i = 0; i < v1.size(); i++)
    {
        if(v1[i] != v2[i]) return 0;
    }

    return 1;
}

template<class T>
int Utility::Convert(const Array<T> & input, const int element_dimension, Array< Array<T> > & output)
{
    if((element_dimension <= 0) || (element_dimension >= input.Dimension()))
    {
        return 0;
    }

    vector<int> input_size;
    input.Size(input_size);

    vector<int> output_size(input.Dimension()-element_dimension);
    vector<int> element_size(element_dimension);

    for(unsigned int i = 0; i < output_size.size(); i++)
    {
        output_size[i] = input_size[i];
    }
    for(unsigned int i = 0; i < element_size.size(); i++)
    {
        element_size[i] = input_size[i + output_size.size()];
    }

    output = Array< Array<T> >(output_size);

    SequentialCounter outer_counter(output_size.size(), vector<int>(output_size.size(), 0), Minus1(output_size));

    SequentialCounter inner_counter(element_size.size(), vector<int>(element_size.size(), 0), Minus1(element_size));

    vector<int> input_index, outer_index, inner_index;
    Array<T> element(element_size);
    T value;
    outer_counter.Reset();
    do
    {
        outer_counter.Get(outer_index);

        inner_counter.Reset();
        do
        {
            inner_counter.Get(inner_index);

            input_index = outer_index;
            input_index.insert(input_index.end(), inner_index.begin(), inner_index.end());
            
            if(! input.Get(input_index, value) )
            {
                return 0;
            }

            if(! element.Put(inner_index, value))
            {
                return 0;
            }
        }
        while(inner_counter.Next());

        if(! output.Put(outer_index, element))
        {
            return 0;
        }
    }
    while(outer_counter.Next());

    return 1;
}
    
template<class T>
int Utility::Convert(const Array< Array<T> > & input, Array<T> & output)
{
    vector<int> input_size;
    input.Size(input_size);

    Array<T> element;
    if(! input.Get(vector<int>(input_size.size(), 0), element)) return 0;

    vector<int> element_size;
    element.Size(element_size);

    SequentialCounter outer_counter(input_size.size(), vector<int>(input_size.size(), 0), Minus1(input_size));
    
    SequentialCounter inner_counter(element_size.size(), vector<int>(element_size.size(), 0), Minus1(element_size));

    vector<int> output_size(input_size);
    output_size.insert(output_size.end(), element_size.begin(), element_size.end());

    output = Array<T>(output_size);

    vector<int> outer_index, inner_index, output_index;
    T value;

    outer_counter.Reset();
    do
    {
        outer_counter.Get(outer_index);

        if(! input.Get(outer_index, element)) return 0;

        if(element.Dimension() != element_size.size()) return 0;
        
        for(int i = 0; i < element.Dimension(); i++)
        {
            if(element.Size(i) != element_size[i]) return 0;
        }

        inner_counter.Reset();
        do
        {
            inner_counter.Get(inner_index);

            if(! element.Get(inner_index, value)) return 0;

            output_index = outer_index;
            output_index.insert(output_index.end(), inner_index.begin(), inner_index.end());

            if(! output.Put(output_index, value)) return 0;
        }
        while(inner_counter.Next());
    }
    while(outer_counter.Next());

    return 1;
}

#endif

