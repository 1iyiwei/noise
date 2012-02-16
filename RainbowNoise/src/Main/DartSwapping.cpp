/*
  DartSwapping.cpp

  Li-Yi Wei
  08/18/2009

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <memory>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "ToroidalBoxDomain.hpp"

#include "GaussianBlob.hpp"
#include "MonomialBlob.hpp"

#include "FairClassWeight.hpp"
#include "SingleClassWeight.hpp"
#include "AllClassWeight.hpp"

#include "UniformFairKernelSize.hpp"
#include "UniformRainbowKernelSize.hpp"

#include "BagEnergySampleStore.hpp"
#include "GridEnergySampleStore.hpp"

#include "UnionPointEnergyField.hpp"

#include "DartSwapper.hpp"
#include "RandomDiscreteDartSwapper.hpp"

#include "ShuffleTools.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 11;
    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << "init_sample_file_name (negative class id for non-sample points) dimension domain_size (dimension numbers) energy_store_method (bag or grid+size_per_unit_domain) swap_method (random) class_weighting_scheme (fair) kernel_type (guassian, monomialn) relative_kernel_size moving_window_size max_num_iterations";
        cerr << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);

    if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension, 1.0);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }

    const string energy_store_method = argv[++arg_ctr];
    const string swap_method = argv[++arg_ctr];
    const string class_weight_scheme = argv[++arg_ctr];
    const string kernel_type = argv[++arg_ctr];
    const float relative_kernel_size = atof(argv[++arg_ctr]);
    const int moving_window_size = abs(atoi(argv[++arg_ctr]));
    const int max_num_iterations = abs(atoi(argv[++arg_ctr]));
   
    // init samples
    vector<Sample> points_and_samples;
    if(! Utility::ReadSamples(dimension, input_file_name, points_and_samples))
    {
        cerr << "cannot read samples from " << input_file_name << endl;
        return 1;
    }

    // extract samples from points
    vector<Sample> samples;
    {
        for(unsigned int i = 0; i < points_and_samples.size(); i++)
        {
            if(points_and_samples[i].id >= 0)
            {
                samples.push_back(points_and_samples[i]);
            }
        }
    }

    // counting
    vector<int> num_samples;    
    if(! Utility::CountSamples(samples, num_samples))
    {
        cerr << "error in counting samples" << endl;
        return 1;
    }

    const int num_class = num_samples.size();

    // domain
    ToroidalBoxDomain domain(domain_size);

    // single blob
    SingleBlob * p_single_blob = 0;
    const string monomial_string = "monomial";
    if(kernel_type == "gaussian")
    {
        p_single_blob = new GaussianBlob;
    }
    else if(kernel_type.find(monomial_string) == 0)
    { 
        const float degree = atof(kernel_type.substr(monomial_string.length(), kernel_type.length()).c_str());
        p_single_blob = new MonomialBlob(degree);
    }
    else
    {
        cerr << "unknown kernel type" << endl;
        return 1;
    }

    if(!p_single_blob)
    {
        throw Exception("null single blob");
    }

    const auto_ptr<SingleBlob> auto_ptr_single_blob(p_single_blob);
    SingleBlob & blob = *auto_ptr_single_blob;

    // class weight
    ClassWeight *p_class_weight = 0;
    
    if(class_weight_scheme == "fair")
    {
        p_class_weight = new FairClassWeight(num_samples);
    }
    else if(class_weight_scheme == "single")
    {
        p_class_weight = new SingleClassWeight(num_samples);
    }
    else if(class_weight_scheme == "all")
    {
#ifdef _USE_2_CLASS_ENERGY
        p_class_weight = new FairClassWeight(num_samples);
#else
        p_class_weight = new AllClassWeight(num_samples);
#endif
    }
    else
    {
        cerr << "unsupported class weight scheme" << endl;
        return 1;
    }

    if(!p_class_weight)
    {
        throw Exception("null class weight");
        return 1;
    }

    auto_ptr<ClassWeight> auto_ptr_class_weight(p_class_weight);
    ClassWeight & class_weight = *auto_ptr_class_weight;

    // kernel size
#ifndef _USE_2_CLASS_ENERGY
    UniformFairKernelSize kernel_size(domain_size, num_samples);
#else
    RMatrix::Method method;
     
    if(class_weight_scheme == "fair")
    {
        method = RMatrix::OPTIMAL;
    }
    else if(class_weight_scheme == "single")
    {
        method = RMatrix::OPTIMAL;
    }
    else if(class_weight_scheme == "all")
    {
        method = RMatrix::GEOMETRIC;
    }
    else
    {
        cerr << "unsupported class weight scheme" << endl;
        return 1;
    }

    UniformRainbowKernelSize kernel_size(domain_size, method, num_samples);
#endif

    if(!kernel_size.SetScale(relative_kernel_size))
    {
        cerr << "cannot set kernel size scale" << endl;
        return 1;
    }

    // energy sample store
    const float kernel_radius_scale = 3; // ought to be enough for Gaussian

    EnergySampleStore * p_energy_store = ShuffleTools::BuildEnergySampleStore(domain, domain_size, kernel_size, energy_store_method, kernel_radius_scale);
    
    if(!p_energy_store)
    {
        throw Exception("null energy sample store");
        return 1;
    }

    auto_ptr<EnergySampleStore> auto_ptr_energy_store(p_energy_store);
    EnergySampleStore & store = *auto_ptr_energy_store;

    // energy field
    UnionPointEnergyField energy_field(domain, blob, num_class, class_weight, kernel_size, store);

    // dart shuffler
    DartSwapper * p_dart_swapper = 0;
    
    if(swap_method == "random")
    {
        p_dart_swapper = new RandomDiscreteDartSwapper(energy_field);
    }
    else
    {
        cerr << "unsupported swap method" << endl;      
        return 1;
    }

    if(!p_dart_swapper)
    {
        throw Exception("null dart swapper");
        return 1;
    }

    auto_ptr<DartSwapper> auto_ptr_dart_swapper(p_dart_swapper);
    DartSwapper & dart_swapper = *auto_ptr_dart_swapper;

    // init random
    Random::InitRandomNumberGenerator();

    // iterations
    if(!dart_swapper.Set(points_and_samples))
    {
        cerr << "cannot set points+samples" << endl;  
        return 1;
    }

    // energy criteria a bit crude; need more investigation
    int iteration = 0;
    int stop = 0; // stop the iterations
    deque<float> moving_window;
    int zero_count = 0;

    for(iteration = 0; !stop && ((max_num_iterations < 0) || (iteration < max_num_iterations)); iteration++)
    {
        const float energy_diff = dart_swapper.Next();

        if(Math::IsNAN(energy_diff))
        {
            cerr << "cannot proceed to the next iteration" << endl;
            return 1;
        }

        if(energy_diff >= 0)
        {
            zero_count++;
        }
        else
        {
            zero_count = 0;
        }

        stop = (zero_count >= moving_window_size);
    }

    cerr << iteration << " iterations" << endl;

    // output
    if(!dart_swapper.Get(points_and_samples))
    {
        cerr << "cannot get samples" << endl;  
        return 1;
    }
    {
        samples.clear();
        for(unsigned int i = 0; i < points_and_samples.size(); i++)
        {
            if(points_and_samples[i].id >= 0)
            {
                samples.push_back(points_and_samples[i]);
            }
        }
    }

    cerr << samples.size() << " samples" << endl;
    if(!Utility::WriteSamples(samples, "cout"))
    {
        cerr << "cannot write out samples" << endl;
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
