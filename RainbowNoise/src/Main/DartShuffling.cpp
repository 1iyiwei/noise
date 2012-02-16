/*
  DartShuffling.cpp

  Li-Yi Wei
  06/19/2009

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <memory>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "ToroidalBoxDomain.hpp"
#include "PlainBoxDomain.hpp"

#include "GaussianBlob.hpp"
#include "MonomialBlob.hpp"

#include "FairClassWeight.hpp"
#include "SingleClassWeight.hpp"
#include "AllClassWeight.hpp"

#include "UniformFairKernelSize.hpp"
#include "UniformRainbowKernelSize.hpp"

#include "BagEnergySampleStore.hpp"
#include "GridEnergySampleStore.hpp"

#include "SampleEnergyField.hpp"
#include "TribePointEnergyField.hpp"
#include "UnionPointEnergyField.hpp"
#include "TrackerEnergyField.hpp"

#include "UnionEnergySampleTracker.hpp"

#include "NullPeakFinder.hpp"
#include "UniformRandomPeakFinder.hpp"

#include "NullTroughFinder.hpp"
#include "UniformRandomTroughFinder.hpp"
#include "UniformSphereRandomTroughFinder.hpp"
#include "DiscreteTroughFinder.hpp"
#include "TrackerTroughFinder.hpp"

#include "DartShuffler.hpp"
#include "MaxPeakDartShuffler.hpp"
#include "GradualPeakDartShuffler.hpp"
#include "ThrowerDartShuffler.hpp"

#include "ShuffleTools.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "Random.hpp"
#include "Exception.hpp"
#include "Timer.hpp"

#ifdef _MEASURE_ENERGY
#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"
#endif

int Main(int argc, char **argv)
{
    // input arguments
#ifdef _MEASURE_ENERGY
    const int argc_add = 4;
#else
    const int argc_add = 0;
#endif

    const int argc_min = 13 + argc_add;
    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << "init_sample_file_name (>= 0 class id for samples, -1 class id for non-sample points, < -1 class id for fixed samples) dimension domain_size (dimension numbers) energy_store_method (bag or grid+size_per_unit_domain) peak_method (random or default) trough_method (random+num_candidates, greedom+num_candidates, trandom+num_candidates, sphererandom+num_candidates, union for union-point, or energy_store_method for tribe-point) shuffle_method (maxpeak/peak1 or gradualpeak/peakN or thrower+threshold) class_weighting_scheme (fair) kernel_type (gaussian, monomial+n) relative_kernel_size moving_window_size max_num_iterations";
#ifdef _MEASURE_ENERGY
        cerr << " energy_class_id energy_array_size energy_image_file_name energy_history_file_name";
#endif
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
    const string peak_method = argv[++arg_ctr];
    const string trough_method = argv[++arg_ctr];
    const string shuffle_method = argv[++arg_ctr];
    const string class_weight_scheme = argv[++arg_ctr];
    const string kernel_type = argv[++arg_ctr];
    const float relative_kernel_size = atof(argv[++arg_ctr]);
    const int moving_window_size = abs(atoi(argv[++arg_ctr]));
    const int max_num_iterations = abs(atoi(argv[++arg_ctr]));

#ifdef _MEASURE_ENERGY
    const int energy_class_id = atoi(argv[++arg_ctr]);
    const int energy_array_size_1d = atoi(argv[++arg_ctr]);
    const string energy_image_file_name = argv[++arg_ctr];
    const string energy_history_file_name = argv[++arg_ctr];
#endif
   
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
            if(points_and_samples[i].id != -1)
            {
                samples.push_back(points_and_samples[i]);
            }
        }
    }

    // build energy points and samples
    vector<EnergySample> energy_points_and_samples(points_and_samples.size());
    {
        for(unsigned int i = 0; i < energy_points_and_samples.size(); i++)
        {
            if(points_and_samples[i].id < 0)
            {
                const bool fixed = ShuffleTools::LoosenClassID(points_and_samples[i].id);

                energy_points_and_samples[i].Fixed(fixed);
            }

            if(! energy_points_and_samples[i].SetSample(points_and_samples[i]))
            {
                throw Exception("cannot set sample for energy point-and-sample");
            }
        }
    }

    // build energy samples
    vector<EnergySample> energy_samples(samples.size());
    {
        for(unsigned int i = 0; i < energy_samples.size(); i++)
        {
            if(samples[i].id < 0)
            {
                const bool fixed = ShuffleTools::LoosenClassID(samples[i].id);
                energy_samples[i].Fixed(fixed);
            }

            if(! energy_samples[i].SetSample(samples[i]))
            {
                throw Exception("cannot set sample for energy sample");
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

    // replicate points to all classes
    // could be updated later to handle the situation
    // where different classes have different point locations
    vector<Sample> points;
    {
        Sample point;

        for(int class_id = 0; class_id < num_class; class_id++)
        {
            for(unsigned int i = 0; i < points_and_samples.size(); i++)
            {
                point = points_and_samples[i];
                point.id = class_id;
                
                points.push_back(point);
            }
        }
    }

    // domain
#if 1
    ToroidalBoxDomain domain(domain_size);
#else
    // debug
    PlainBoxDomain domain(domain_size);
#endif

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
        // default to Gaussian
        p_single_blob = new GaussianBlob;
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

    const auto_ptr<ClassWeight> auto_ptr_class_weight(p_class_weight);
    ClassWeight & class_weight = *auto_ptr_class_weight;

    // kernel size
#ifndef _USE_2_CLASS_ENERGY
    UniformFairKernelSize kernel_size(domain_size, num_samples);
#else
    RMatrix::Method method;
     
    if(class_weight_scheme == "fair")
    {
        if(kernel_type.find("uniform") == 0)
        {
            method = RMatrix::UNIFORM;
        }
        else
        {
            method = RMatrix::OPTIMAL;
        }
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

    const auto_ptr<EnergySampleStore> auto_ptr_energy_store(p_energy_store);
    EnergySampleStore & store = *auto_ptr_energy_store;

    // points energy_store
    EnergySampleStore * p_points_energy_store = 0;

    if(trough_method.find("grid") < trough_method.size())
    {
        p_points_energy_store = ShuffleTools::BuildEnergySampleStore(domain, domain_size, kernel_size, trough_method, kernel_radius_scale);
    }

    const auto_ptr<EnergySampleStore> auto_ptr_points_energy_store(p_points_energy_store);

    // energy sample tracker
    UnionEnergySampleTracker * p_energy_sample_tracker = 0;

    const int tracker_grid_scale = 1;
    const string trandom_string = "trandom";
    if(trough_method.find(trandom_string) == 0)
    {
        p_energy_sample_tracker = ShuffleTools::BuildEnergySampleTracker(domain_size, num_samples, tracker_grid_scale);
    }

    const auto_ptr<EnergySampleTracker> auto_ptr_energy_sample_tracker(p_energy_sample_tracker);
    
    // energy field
    EnergyField * p_energy_field = 0;
    TribePointEnergyField * p_tribe_point_energy_field = 0;
    TrackerEnergyField * p_tracker_energy_field = 0;

    if(! p_points_energy_store)
    {
        if(trough_method.find("union") == 0)
        {
            p_energy_field = new UnionPointEnergyField(domain, blob, num_class, class_weight, kernel_size, store);
        }
        else
        {
            p_energy_field = new SampleEnergyField(domain, blob, num_class, class_weight, kernel_size, store);

            if(auto_ptr_energy_sample_tracker.get())
            {
                p_tracker_energy_field = new TrackerEnergyField(*p_energy_field, *auto_ptr_energy_sample_tracker);
            }
        }
    }
    else
    {
        p_energy_field = p_tribe_point_energy_field = new TribePointEnergyField(domain, blob, num_class, class_weight, kernel_size, points, *p_points_energy_store, store);
    }

    if(!p_energy_field)
    {
        throw Exception("null energy field");
        return 1;
    }

    const auto_ptr<EnergyField> auto_ptr_energy_field(p_energy_field);
    const auto_ptr<TrackerEnergyField> auto_ptr_tracker_energy_field(p_tracker_energy_field);

    EnergyField & energy_field = (auto_ptr_tracker_energy_field.get() == 0 ? *auto_ptr_energy_field : *dynamic_cast<EnergyField *>(auto_ptr_tracker_energy_field.get()));

    // peak finder
    PeakFinder * p_peak_finder = 0;
    if(peak_method == "random")
    {
        p_peak_finder = new UniformRandomPeakFinder;
    }
    else
    {
        p_peak_finder = new NullPeakFinder;
    }

    if(!p_peak_finder)
    {
        throw Exception("null peak finder");
        return 1;
    }

    const auto_ptr<PeakFinder> auto_ptr_peak_finder(p_peak_finder);
    PeakFinder & peak_finder = *auto_ptr_peak_finder;

    // trough finder
    TroughFinder * p_trough_finder = 0;

    //if(trough_method.find("random") != string::npos)
    if(trough_method.find("random") == 0)
    {
        const string name = "random";
        const int num_candidates = atoi(trough_method.substr(name.length(), trough_method.length()).c_str());

        // cerr << "num_candidates " << num_candidates << endl;
        p_trough_finder = new UniformRandomTroughFinder(num_candidates, 0);
    }
    else if(trough_method.find("greedom") == 0)
    {
        const string name = "greedom";
        const int num_candidates = atoi(trough_method.substr(name.length(), trough_method.length()).c_str());

        // cerr << "num_candidates " << num_candidates << endl;
        p_trough_finder = new UniformRandomTroughFinder(num_candidates, 1);
    }
    else if(trough_method.find("sphererandom") == 0)
    {
        const string name = "sphererandom";
        const int num_candidates = atoi(trough_method.substr(name.length(), trough_method.length()).c_str());

        float domain_volume = 1.0;
        for(unsigned int i = 0; i < domain_size.size(); i++)
        {
            domain_volume *= domain_size[i];
        }

        vector<float> radius(num_samples.size());
        for(unsigned int i = 0; i < radius.size(); i++)
        {
            radius[i] = 0.5*Math::ComputeMaxMinDistance(dimension, num_samples[i]/domain_volume);
        }

        p_trough_finder = new UniformSphereRandomTroughFinder(num_class, num_candidates, 0, radius);
    }
    else if(auto_ptr_tracker_energy_field.get())
    {
        const int num_candidates = atoi(trough_method.substr(trandom_string.length(), trough_method.length()).c_str());

        p_trough_finder = new TrackerTroughFinder(*auto_ptr_tracker_energy_field, num_candidates, 0);
    }
    else if(p_tribe_point_energy_field)
    {
       p_trough_finder = new DiscreteTroughFinder(*p_tribe_point_energy_field);
    }
    else
    { 
        p_trough_finder = new NullTroughFinder();
    }

    if(!p_trough_finder)
    {
        throw Exception("null trough finder");
        return 1;
    }

    const auto_ptr<TroughFinder> auto_ptr_trough_finder(p_trough_finder);
    TroughFinder & trough_finder = *auto_ptr_trough_finder;

    // dart shuffler
    DartShuffler * p_dart_shuffler = 0;

    const string peak_string("peak");
    const string gradual_peak_string("gradualpeak");
    const string thrower_string("thrower");
    if(shuffle_method.find(peak_string) == 0)
    {
        const int num_peaks_to_look = atoi(shuffle_method.substr(peak_string.length(), shuffle_method.length()).c_str());

        if(num_peaks_to_look == 1)
        {
            p_dart_shuffler = new MaxPeakDartShuffler(energy_field, peak_finder, trough_finder);
        }
        else
        {
            p_dart_shuffler = new GradualPeakDartShuffler(energy_field, peak_finder, trough_finder, num_peaks_to_look-1);
        }
    }
    else if(shuffle_method == gradual_peak_string)
    {
        p_dart_shuffler = new GradualPeakDartShuffler(energy_field, peak_finder, trough_finder, -1);
    }
    else if(shuffle_method.find(gradual_peak_string) == 0)
    {
        const int num_peaks_to_look = atoi(shuffle_method.substr(gradual_peak_string.length(), shuffle_method.length()).c_str());
        
        p_dart_shuffler = new GradualPeakDartShuffler(energy_field, peak_finder, trough_finder, num_peaks_to_look);
    }
    else if(shuffle_method.find(thrower_string) == 0)
    {
        p_dart_shuffler = new ThrowerDartShuffler(energy_field, peak_finder, trough_finder);

        const float threshold_energy = atof(shuffle_method.substr(thrower_string.length(), shuffle_method.length()).c_str());

        if(threshold_energy > 0)
        {
            trough_finder.ThresholdEnergy(threshold_energy);
        }
    }
    else
    {
        p_dart_shuffler = new MaxPeakDartShuffler(energy_field, peak_finder, trough_finder);
    }

    if(!p_dart_shuffler)
    {
        throw Exception("null dart shuffler");
        return 1;
    }

    const auto_ptr<DartShuffler> auto_ptr_dart_shuffler(p_dart_shuffler);
    DartShuffler & dart_shuffler = *auto_ptr_dart_shuffler;

    // init random
    Random::InitRandomNumberGenerator();

    // iterations
    if(((trough_method.find("union") == 0) && !dart_shuffler.Set(energy_points_and_samples)) || ((trough_method.find("union") != 0) &&!dart_shuffler.Set(energy_samples)))
    {
        cerr << "cannot set samples" << endl;  
        return 1;
    }

    // energy criteria a bit crude; need more investigation
    int iteration = 0;
    int stop = 0; // stop the iterations
    float energy = Math::INF;
    deque<float> moving_window;
    float moving_sum = 0;
#ifdef _MEASURE_ENERGY
    vector<float> energy_history;
#endif

    Timer timer;
    timer.Start();

    for(iteration = 0; !stop && ((max_num_iterations < 0) || (iteration < max_num_iterations)); iteration++)
    {
        const float energy_updated = dart_shuffler.Next();
        if(Math::IsNAN(energy_updated))
        {
            cerr << "cannot proceed to the next iteration" << endl;
            return 1;
        }

        if(Math::IsINF(energy_updated))
        {
            break;
        }

        const float energy_new = dart_shuffler.PeakEnergy();
        stop = 0;

        if(moving_window.size() >= moving_window_size)
        {
            stop += (moving_window.size()*energy_new > moving_sum);
        }

        moving_window.push_back(energy_new);
        moving_sum += energy_new;

        if(moving_window.size() > moving_window_size)
        {
            moving_sum -= moving_window[0];
            moving_window.pop_front();
        }
    
        energy = energy_new;

#ifdef _MEASURE_ENERGY
        energy_history.push_back(energy);
#endif
    }

    timer.Stop();
    const double total_time = timer.ElapsedTime();
    cerr << "total time " << total_time << endl;

    cerr << iteration << " iterations" << endl;

    // output
    if(!dart_shuffler.Get(energy_samples))
    {
        cerr << "cannot get samples" << endl;  
        return 1;
    }

    samples = vector<Sample>(energy_samples.size());
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!energy_samples[i].GetSample(samples[i]))
        {
            throw Exception("cannot get sample from energy sample");
        }

        if(energy_samples[i].Fixed())
        {
            if(!ShuffleTools::FastenClassID(samples[i].id))
            {
                throw Exception("cannot fasten class id");
            }
        }
    }

    if(trough_method.find(string("union")) == 0)
    {
        vector<Sample> real_samples;

        for(unsigned int i = 0; i < samples.size(); i++)
        {
            if(samples[i].id >= 0)
            {
                real_samples.push_back(samples[i]);
            }
        }

        samples = real_samples;
    }

    cerr << samples.size() << " samples" << endl;
    if(!Utility::WriteSamples(samples, "cout"))
    {
        cerr << "cannot write out samples" << endl;
        return 1;
    }
  
#ifdef _MEASURE_ENERGY
    // energy evaluation
    const vector<int> output_size(dimension, energy_array_size_1d);
    Array<FrameBuffer::PF> energy_array(output_size);
    Sample sample(dimension);
    sample.id = energy_class_id;
    FrameBuffer::PF pixel;

    SequentialCounter counter(dimension, 0, energy_array_size_1d-1);
    vector<int> index(dimension);

    counter.Reset();
    do
    {
        counter.Get(index);

        for(int i = 0; i < sample.coordinate.Dimension(); i++)
        {
            sample.coordinate[i] = (index[i]+0.5)*domain_size[i]/energy_array_size_1d;
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
    if(! FrameBuffer::WritePFM(energy_array, energy_image_file_name.c_str()))
    {
        cerr << "cannot write to " << energy_image_file_name << endl; 
        return 1;
    }

    ofstream energy_history_output(energy_history_file_name.c_str());

    if(! energy_history_output)
    {
        cerr << "cannot write energy history to " << energy_history_file_name << endl;
        return 1;
    }
    else
    {
        for(unsigned int i = 0; i < energy_history.size(); i++)
        {
            energy_history_output << energy_history[i] << " ";
        }
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
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}

