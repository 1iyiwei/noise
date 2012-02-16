/*
  DartEnergy.cpp

  measure the energy for a set of darts

  Li-Yi Wei
  07/01/2009

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "ToroidalBoxDomain.hpp"

#include "GaussianBlob.hpp"

#include "FairClassWeight.hpp"

#include "UniformFairKernelSize.hpp"
#include "UniformRainbowKernelSize.hpp"

#include "BagEnergySampleStore.hpp"

#include "SampleEnergyField.hpp"

#include "NullPeakFinder.hpp"
#include "UniformRandomTroughFinder.hpp"

#include "Utility.hpp"
#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    throw Exception("DartEnergy is deprecated. Use DartShufflingEnergy!");

    // input arguments
    const int argc_min = 7;
    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " sample_file_name output_file_name dimension class_weighting_scheme (fair) relative_kernel_size output_size" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const string output_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);
    const vector<float> domain_size(2, 1.0); // hardwired for now
    const string class_weight_scheme = argv[++arg_ctr];
    const float relative_kernel_size = atof(argv[++arg_ctr]);
    const int output_size_1d = atoi(argv[++arg_ctr]);

    // samples
    vector<Sample> samples;

    if(! Utility::ReadSamples(dimension, input_file_name, samples))
    {
        cerr << "cannot read samples from " << input_file_name << endl;
        return 1;
    }

    // counting
    vector<int> num_samples;
    
    if(! Utility::CountSamples(samples, num_samples))
    {
        cerr << "error in counting samples" << endl;
        return 1;
    }

    // domain
    ToroidalBoxDomain domain(domain_size);

    // single blob
    GaussianBlob blob;

    // class weight
    FairClassWeight class_weight(num_samples);

    // kernel size
    UniformFairKernelSize kernel_size(domain_size, num_samples);
    if(!kernel_size.SetScale(relative_kernel_size))
    {
        cerr << "cannot set kernel size scale" << endl;
        return 1;
    }

    // energy sample store
    BagEnergySampleStore store;

    // energy field
    SampleEnergyField sample_energy_field(domain, blob, num_samples.size(), class_weight, kernel_size, store);

    EnergyField & energy_field = sample_energy_field;

    if(! energy_field.Set(samples))
    {
        cerr << "cannot set samples into the energy field" << endl;
        return 1;
    }

    // evaluation
    const vector<int> output_size(dimension, output_size_1d);
    Array<FrameBuffer::PF> energy_array(output_size);
    Sample sample(dimension);
    FrameBuffer::PF pixel;

    SequentialCounter counter(dimension, 0, output_size_1d-1);
    vector<int> index(dimension);

    counter.Reset();
    do
    {
        counter.Get(index);

        for(int i = 0; i < sample.coordinate.Dimension(); i++)
        {
            sample.coordinate[i] = (index[i]+0.5)*domain_size[i]/output_size_1d;
        }

        const float energy_value = energy_field.Get(sample);

        pixel.r = pixel.g = pixel.b = energy_value;
        if(! energy_array.Put(index, pixel))
        {
            cerr << "cannot put energy pixel" << endl;
            return 1;
        }
    }
    while(counter.Next());
    
    // output
    if(! FrameBuffer::WritePFM(energy_array, output_file_name.c_str()))
    {
        cerr << "cannot write to " << output_file_name << endl; 
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
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
