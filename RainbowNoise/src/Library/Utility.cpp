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
#include "Exception.hpp"
#include "Math.hpp"
#include "FrameBuffer.hpp"
#include "UniformDistanceField.hpp"
#include "RampDistanceField.hpp"
#include "SampledDistanceField.hpp"

int Utility::ReadSamples(const int dimension, const string & input_file_name, vector<Sample> & samples)
{
    return ReadSamples(SAMPLE_ASCII, dimension, input_file_name, samples);
}

int Utility::WriteSamples(const vector<Sample> & samples, const string & output_file_name)
{
    return WriteSamples(SAMPLE_ASCII, samples, output_file_name);
}

int Utility::ReadSamples(const SampleMode & mode, const int dimension, const string & input_file_name, vector<Sample> & samples)
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
            input >> sample.id;

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

int Utility::WriteSamples(const SampleMode & mode, const vector<Sample> & samples, const string & output_file_name)
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
            if(is_cout)
                cout << samples[i].id;
            else
                output << samples[i].id;

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
            if(is_cout)
                cout << samples[i].id;
            else
                output << samples[i].id;

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

int Utility::ReadDistanceField(const int dimension, const string & distance_field_file_name, vector< Array<float> > & distance_field_data)
{
    if((distance_field_data.size() != 1) && (distance_field_data.size() != 3))
    {
        return 0;
    }

    // read in image, recording importance
    Array<FrameBuffer::PF> image;

    if(! FrameBuffer::ReadPFM(distance_field_file_name.c_str(), image))
    {
        return 0;
    }

    if(image.Dimension() != dimension) return 0;
    
    // convert into gray scale importance
    vector<int> field_size;
    image.Size(field_size);

    for(unsigned int k = 0; k < distance_field_data.size(); k++)
    {
        distance_field_data[k] = Array<float>(field_size);
    }

    vector<FrameBuffer::PF> input_data;
    vector<float> output_data;

    if(! image.Get(input_data)) return 0;
    if(! distance_field_data[0].Get(output_data)) return 0;

    assert(input_data.size() == output_data.size());

    if(distance_field_data.size() == 1)
    {
        for(unsigned int i = 0; i < output_data.size(); i++)
        {
            output_data[i] = input_data[i].g;
        }
    
        if(! distance_field_data[0].Put(output_data)) return 0;
    }
    else if(distance_field_data.size() == 3)
    {
        for(unsigned int k = 0; k < distance_field_data.size(); k++)
        {
            for(unsigned int i = 0; i < output_data.size(); i++)
            {
                output_data[i] = (k == 0 ? input_data[i].r : (k == 1 ? input_data[i].g : input_data[i].b));
            }
    
            if(! distance_field_data[k].Put(output_data)) return 0;
        }
    }
    else
    {
        return 0;
    }

    // done
    return 1;
}

const DistanceField * Utility::GetDistanceField(const vector<float> & domain_size, const vector<float> & min_distances, const string & importance_file_name)
{
    DistanceField * p_distance_field = 0;
    if(!strstr(importance_file_name.c_str(), ".pfm"))
    {
        if(strstr(importance_file_name.c_str(), "uniform"))
        {
            p_distance_field = new UniformDistanceField(domain_size, min_distances);
        }
        else if(strstr(importance_file_name.c_str(), "ramp"))
        {
            p_distance_field = new RampDistanceField(domain_size, min_distances);
        }
        else
        {
            throw Exception("unknown built-in distance field");
            return 0;
        }
    }
    else
    {
        vector< Array<float> > importances(min_distances.size());
        if(! ReadDistanceField(domain_size.size(), importance_file_name, importances))
        {
            throw Exception("error in reading importance file name"); //<< importance_file_name << endl;
            return 0;
        }
        
        p_distance_field = new SampledDistanceField(domain_size, min_distances, importances);
    }

    return p_distance_field;
}

const SampledDistanceField * Utility::GetDistanceField(const vector<float> & domain_size, const vector<float> & min_distances, const vector<string> & importance_file_names)
{   
    if(min_distances.size() != importance_file_names.size())
    {
        throw Exception("min_distances.size() != importance_file_names.size()");
        return 0;
    }

    SampledDistanceField * p_distance_field = 0;

    vector< Array<float> > importances(min_distances.size());

    for(unsigned int i = 0; i < importances.size(); i++)
    {
        vector< Array<float> > importance(1);

        if(! ReadDistanceField(domain_size.size(), importance_file_names[i], importance))
        {
            throw Exception("error in reading importance file name"); //<< importance_file_name << endl;
            return 0;
        }

        importances[i] = importance[0];
    }
   
    p_distance_field = new SampledDistanceField(domain_size, min_distances, importances);

    return p_distance_field;
}
 
int Utility::NormalizeRValues(const vector<float> & domain_size, const vector<string> & importance_file_names, const vector<float> & input_min_distances, vector<float> & output_min_distances)
{
    if(importance_file_names.size() != input_min_distances.size())
    {
        // size mismatch
        return 0;
    }

    // read data
    vector< Array<float> > importances(importance_file_names.size());
    for(unsigned int i = 0; i < importances.size(); i++)
    {
        vector< Array<float> > importance(1);

        if(! ReadDistanceField(domain_size.size(), importance_file_names[i], importance))
        {
            throw Exception("error in reading importance file name"); //<< importance_file_name << endl;
            return 0;
        }

        importances[i] = importance[0];
    }
   
    // compute averages
    vector<float> averages(importances.size());
    float global_average = 0;
    vector<float> importance_data;
    for(unsigned int which_data = 0; which_data < averages.size(); which_data++)
    {
        const Array<float> & importance = importances[which_data];

        if(! importance.Get(importance_data))
        {
            throw Exception("SampledDistanceField::SampledDistanceField() : cannot get importance data");
        }

        averages[which_data] = 0;
        for(unsigned int i = 0; i < importance_data.size(); i++)
        {
            averages[which_data] += importance_data[i];
        }
        averages[which_data] /= importance_data.size();

        global_average += averages[which_data];
    }
    global_average /= averages.size();

    // perform normalization
    const int dimension = domain_size.size();
    output_min_distances = input_min_distances;

    for(unsigned int i = 0; i < output_min_distances.size(); i++)
    {
        if(averages[i] <= 0) return 0;
        output_min_distances[i] = input_min_distances[i]*pow(global_average/averages[i]*1.0, 1.0/dimension);
    }

    // done
    return 1;
}
