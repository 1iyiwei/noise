/*
  Utility.cpp

  Li-Yi Wei
  08/26/2008

*/

#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <math.h>

#include "Utility.hpp"
#include "FrameBuffer.hpp"
#include "Random.hpp"
#include "Exception.hpp"
#include "Math.hpp"

vector<string> Utility::Convert(int argc, char *argv[])
{
    vector<string> answer(max(0, argc));

    for(unsigned int i = 0; i < answer.size(); i++)
    {
        answer[i] = argv[i];
    }

    return answer;
}

Coordinate Utility::Diff(const Coordinate & c1, const Coordinate & c2)
{
    if(c1.Dimension() != c2.Dimension()) throw Exception("Utility::Diff(): dimension mismatch");

    Coordinate result(c1);

    for(int i = 0; i < result.Dimension(); i++)
    {
        result[i] -= c2[i];
    }

    return result;
}

Coordinate Utility::Add(const Coordinate & p1, const Coordinate & p2)
{
    if(p1.Dimension() != p2.Dimension()) throw Exception("Utility::Add(): p1.Dimension() != p2.Dimension()");

    Coordinate result(p1);

    for(int i = 0; i < result.Dimension(); i++)
    {
        result[i] = p1[i] + p2[i];
    }

    return result;
}

float Utility::Dot(const Coordinate & c1, const Coordinate & c2)
{
    if(c1.Dimension() != c2.Dimension()) throw Exception("Utility::Dot(): dimension mismatch");

    float result = 0;
    
    for(int i = 0; i < c1.Dimension(); i++)
    {
        result += c1[i]*c2[i];
    }

    return result;
}

Coordinate Utility::Multiply(const Coordinate & p1, const CoordinateType & c)
{
    Coordinate result(p1);

    for(int i = 0; i < result.Dimension(); i++)
    {
        result[i] = p1[i]*c;
    }

    return result;
}

float Utility::Distance2(const Coordinate & c1, const Coordinate & c2)
{
    if(c1.Dimension() != c2.Dimension()) throw Exception("Utility::Distance2(): dimension mismatch");

    float distance = 0;

    for(int i = 0; i < c1.Dimension(); i++)
    {
        const float diff = c1[i] - c2[i];
        distance += diff*diff;
    }

    return distance;
}

float Utility::Norm2(const Coordinate & c)
{
    float norm2 = 0;

    for(int i = 0; i < c.Dimension(); i++)
    {
        norm2 += c[i]*c[i];
    }

    return norm2;
}

float Utility::Distancep(const CoordinateType & p, const Coordinate & c1, const Coordinate & c2)
{
    if(c1.Dimension() != c2.Dimension()) throw Exception("Utility::Distance2(): dimension mismatch");

    float distance = 0;

    for(int i = 0; i < c1.Dimension(); i++)
    {
        const float diff = fabs(c1[i] - c2[i]);
        distance += pow(diff, p);
    }

    return distance;
}

float Utility::DistanceINF(const Coordinate & c1, const Coordinate & c2)
{
    if(c1.Dimension() != c2.Dimension()) throw Exception("Utility::Distance2(): dimension mismatch");

    float distance = 0;

    for(int i = 0; i < c1.Dimension(); i++)
    {
        const float diff = fabs(c1[i] - c2[i]);
        if(distance < diff)
        {
            distance = diff;
        }
    }

    return distance;
}

vector<int> Utility::Minus1(const vector<int> & input)
{
    vector<int> output(input);

    for(unsigned int i = 0; i < output.size(); i++)
    {
        output[i] = input[i]-1;
    }

    return output;
}

bool Utility::Inside(const vector<float> & domain, const Sample & sample)
{
    return Inside(vector<float>(domain.size(), 0), domain, sample);
}

bool Utility::Inside(const vector<float> & domain_min, const vector<float> & domain_max, const Sample & sample)
{
    if((domain_min.size() != domain_max.size()) || (domain_max.size() != sample.coordinate.Dimension())) throw Exception("dimension mismatch");

    for(unsigned int i = 0; i < domain_min.size(); i++)
    {
        if((sample.coordinate[i] < domain_min[i]) || (sample.coordinate[i] >= domain_max[i]))
        {
            return 0;
        }
    }

    return 1;
}

int Utility::ReadSamples(const int dimension, const string & input_file_name, vector<Sample> & samples)
{
    return ReadSamples(1, dimension, input_file_name, samples);
}

int Utility::WriteSamples(const vector<Sample> & samples, const string & output_file_name)
{
    const SampleMode output_mode = (((output_file_name.find(".txt") == string::npos) && (output_file_name != "cout")) ? SAMPLE_BINARY : SAMPLE_ASCII);

    return WriteSamples(output_mode, 1, samples, output_file_name);
}

int Utility::ReadSamples(const int has_id, const int dimension, const string & input_file_name, vector<Sample> & samples)
{
    const SampleMode input_mode = (((input_file_name.find(".txt") == string::npos) && (input_file_name != "cin")) ? SAMPLE_BINARY : SAMPLE_ASCII);

    return ReadSamples(input_mode, has_id, dimension, input_file_name, samples);
}

int Utility::ReadSamples(const SampleMode & mode,  const int has_id, const int dimension, const string & input_file_name, vector<Sample> & samples)
{
    ifstream input(input_file_name.c_str());
    samples.clear();

    if(! input)
    {
        return 0;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
            if(has_id)
            {
                input >> sample.id;
            }

            for(int i = 0; i < dimension; i++)
            {
                if(mode == SAMPLE_ASCII)
                {
                    input >> sample.coordinate[i];
                }
                else
                {
                    unsigned long foo;
                    float bar;
                    input >> foo;
                    bar = *(reinterpret_cast<float *>(&foo));
                    sample.coordinate[i] = bar;
                }
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    return 1;
}

int Utility::WriteSamples(const SampleMode & mode, const int has_id, const vector<Sample> & samples, const string & output_file_name)
{
    const int is_cout = (output_file_name == "cout");

    ofstream output;

    if(!is_cout)
    {
        output.open(output_file_name.c_str());
    }

    if(!is_cout && !output)
    {
        return 0;
    }
    else if(mode == SAMPLE_ASCII)
    {
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            if(has_id)
            {
                if(is_cout)
                    cout << samples[i].id;
                else
                    output << samples[i].id;
            }

            for(int j = 0; j < samples[i].coordinate.Dimension(); j++)
            {
                if(is_cout)
                    cout << " " << samples[i].coordinate[j];
                else
                    output << " " << samples[i].coordinate[j];
            }

            if(is_cout)
                cout << endl;
            else
                output << endl;
        }
    }
    else
    {
        if(sizeof(unsigned long) != sizeof(float)) throw Exception("sizeof(unsigned long) != sizeof(float)");
        
        unsigned long foo = 0;
        float bar = 0;

        for(unsigned int i = 0; i < samples.size(); i++)
        { 
            if(has_id)
            {
                if(is_cout)
                    cout << samples[i].id;
                else
                    output << samples[i].id;
            }

            for(int j = 0; j < samples[i].coordinate.Dimension(); j++)
            {
                bar = samples[i].coordinate[j];
                foo = *(reinterpret_cast<unsigned long *>(&bar));
             
                if(is_cout)
                    cout << " " << foo;
                else
                    output << " " << foo;
            }

            if(is_cout)
                cout << endl;
            else
                output << endl;
        }
    }

    return 1;
}

int Utility::CountSamples(const vector<Sample> & samples, vector<int> & num_samples)
{
    int max_class_id = -1;

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(samples[i].id > max_class_id)
        {
            max_class_id = samples[i].id;
        }
    }

    const int num_classes = max_class_id + 1;
    num_samples = vector<int>(num_classes, 0);

    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if((samples[i].id < 0) || (samples[i].id >= num_samples.size()))
        {
            return 0;
        }
        else
        {
            num_samples[samples[i].id]++;
        }
    }

    // done
    return 1;
}

string Utility::BuildPalette(const int num_colors, const string color_palette_file_name, vector<FrameBuffer::P3> & palette)
{
    palette = vector<FrameBuffer::P3> (num_colors);

    const FrameBuffer::P3 fixed_palette[] = 
        {
            {255, 0, 0}, {0, 153, 0}, {0, 0, 255},
            {0, 255, 255}, {255, 0, 255}, {255, 255, 0}
        };

    vector<FrameBuffer::P3> given_palette;

    if(color_palette_file_name == "")
    {
        for(int i = 0; i < sizeof(fixed_palette)/sizeof(FrameBuffer::P3); i++)
        {
            given_palette.push_back(fixed_palette[i]);
        }
    }
    else
    {
        ifstream input(color_palette_file_name.c_str());

        if(! input)
        {
            return "cannot read from " + color_palette_file_name;
        }
        else
        {
            FrameBuffer::P3 color;
            while(input.good())
            {
                input >> color.r;
                input >> color.g;
                input >> color.b;

                if(input.good()) given_palette.push_back(color);
            }
        }
    }

    // cerr << "given color: " << given_palette.size() << ", total color: " << num_colors << endl;

    for(unsigned int i = 0; i < palette.size(); i++)
    {
        if(i < given_palette.size())
        {
            palette[i] = given_palette[i];
        }
        else
        {
            palette[i].r = static_cast<int>(floor(256*Random::UniformRandom()));
            palette[i].g = static_cast<int>(floor(256*Random::UniformRandom()));
            palette[i].b = static_cast<int>(floor(256*Random::UniformRandom()));
        }
    }

    return "";
}

string Utility::Normalize(Array<float> & importance, const vector<float> & domain_size)
{
    if(importance.Dimension() != domain_size.size())
    {
        return "dimension mismatch";
    }

    vector<float> cell_size(domain_size);
    for(unsigned int i = 0; i < cell_size.size(); i++)
    {
        cell_size[i] = domain_size[i]/importance.Size(i);
    }

    float cell_volume = 1.0;
    for(unsigned int i = 0; i < cell_size.size(); i++)
    {
        cell_volume *= cell_size[i];
    }
    
    vector<float> values;
    importance.Get(values);
    
    float sum = 0;
    for(unsigned int i = 0; i < values.size(); i++)
    {
        sum += values[i];
    }
    
    if(sum <= 0) return "non-positive sum";
    
    for(unsigned int i = 0; i < values.size(); i++)
    {
        values[i] /= sum*cell_volume;
    }
    
    importance.Put(values);

    return "";
}

int Utility::ReadMatrixArray(const string & input_file_name, Array<Matrix> & matrix_array)
{
    Array<CoordinateType> component_array;

    if(! FrameBuffer::ReadF2M(input_file_name, component_array))
    {
        throw Exception("Utility::ReadMatrixArray(): cannot read component array");
        return 0;
    }

    Array< Array<CoordinateType> > array_array;

    if(! Convert(component_array, Matrix::MatrixDimension(), array_array))
    {
        throw Exception("Utility::ReadMatrixArray(): cannot convert component array into array array");
        return 0;
    }

    vector<int> size;
    array_array.Size(size);

    matrix_array = Array<Matrix>(size);

    vector< Array<CoordinateType> > arrays;
    if(! array_array.Get(arrays)) return 0;

    vector<Matrix> matrices(arrays.size());
    for(unsigned int i = 0; i < matrices.size(); i++)
    {
        matrices[i] = arrays[i];
    }

    if(! matrix_array.Put(matrices)) return 0;

    return 1;
}

int Utility::WriteMatrixArray(const Array<Matrix> & matrix_array, const string & output_file_name)
{
    vector<int> size;
    matrix_array.Size(size);

    Array< Array<CoordinateType> > array_array(size);

    vector<Matrix> matrices;
    if(! matrix_array.Get(matrices)) return 0;

    vector< Array<CoordinateType> > arrays(matrices.size());

    for(unsigned int i = 0; i < arrays.size(); i++)
    {
        arrays[i] = matrices[i];
    }

    if(! array_array.Put(arrays)) return 0;

    Array<CoordinateType> component_array;

    if(! Convert(array_array, component_array)) return 0;

    if(! FrameBuffer::WriteF2M(component_array, output_file_name)) return 0;

    return 1;
}
