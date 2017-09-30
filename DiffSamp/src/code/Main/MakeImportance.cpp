/*
  MakeImportance.cpp

  generate synthetic importance fields

  Li-Yi Wei
  10/11/2008

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>

#include "ImportanceField.hpp"
#include "SampledImportanceField.hpp"
#include "BalzerImportanceField.hpp"
#include "CosImportanceField.hpp"

#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 8;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension resolution_per_unit_domain (pixels) domain_size (dimension_numbers) domain_offset (dimension_numbers) range_offset (scalar, relative to the maximum range value) output_file_name.f2m option [option specific arguments]" << endl;

        return 1;
    }
    
    int arg_ctr = 0;
    const int dimension = atoi(argv[++arg_ctr]);
    const int resolution_per_unit_domain = atoi(argv[++arg_ctr]);
    const float cell_size = 1.0/resolution_per_unit_domain;

    if(argc < (min_argc + 2*(dimension - 1)))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }

    vector<float> domain_offset(dimension);
    for(unsigned int i = 0; i < domain_offset.size(); i++)
    {
        domain_offset[i] = atof(argv[++arg_ctr]);
    }

    const float range_offset = atof(argv[++arg_ctr]);

    const string output_file_name = argv[++arg_ctr];

    const string option = argv[++arg_ctr];

    // importance field
    ImportanceField * p_importance_field = 0;

    if(option == "balzer")
    {
        if(arg_ctr + 3 > argc-1)
        {
            cerr << "Usage: " << option << " exp_value sin_weight sin_freq" << endl;
            return 1;
        }

        const float exp_value = atof(argv[++arg_ctr]);
        const float sin_weight = atof(argv[++arg_ctr]);
        const float sin_freq = atof(argv[++arg_ctr]);

        p_importance_field = new BalzerImportanceField(dimension, exp_value, sin_weight, sin_freq);
    }
    else if(option == "cos")
    {
        if(arg_ctr + 1 > argc-1)
        {
            cerr << "Usage: " << option << " frequency" << endl;
            return 1;
        }

        const float frequency_1d = atof(argv[++arg_ctr]);

        Coordinate frequency(dimension);
        frequency.Clear(0);

        frequency[0] = frequency_1d;

        p_importance_field = new CosImportanceField(frequency);

    }
    else if(option.find(".f2m") != string::npos)
    {
        Array<float> importance;

        if(! FrameBuffer::ReadF2M(option, importance))
        {
            cerr << "cannot read from " << option << endl;
            return 1;
        }

        p_importance_field = new SampledImportanceField(importance, cell_size);
    }
    else
    {
        cerr << "unknown importance field option" << endl;
        return 1;
    }

    if(!p_importance_field) throw Exception("!p_importance_field");

    const ImportanceField & importance_field = *p_importance_field;

    // produce output
    vector<int> output_size(dimension);
    for(unsigned int i = 0; i < output_size.size(); i++)
    {
        output_size[i] = resolution_per_unit_domain*domain_size[i];
    }

    Array<FrameBuffer::F2> output(output_size);

    SequentialCounter counter(dimension, vector<int>(dimension, 0), Utility::Minus1(output_size));

    counter.Reset();

    vector<int> index;
    Sample sample(dimension);
    float max_value = 0;
    do
    {
        counter.Get(index);

        for(int i = 0; i < sample.coordinate.Dimension(); i++)
        {
            sample.coordinate[i] = index[i]*cell_size + domain_offset[i];
        }

        const float value = importance_field.Query(sample);

        if(value > max_value)
        {
            max_value = value;
        }

        if(! output.Put(index, value)) throw Exception("cannot put output value");
    }
    while(counter.Next());

    // offset
    {
        const float offset = range_offset * max_value;

        vector<float> values;
        output.Get(values);

        for(unsigned int i = 0; i < values.size(); i++)
        {
            values[i] += offset;
        }

        output.Put(values);
    }

    // write output
    if(! FrameBuffer::WriteF2M(output, output_file_name))
    {
        cerr << "cannot write to " << output_file_name << endl;
        return 1;
    }

    // clean up
    if(p_importance_field)
    {
        delete p_importance_field;
        p_importance_field = 0;
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
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
