/*
  DiffXform.cpp

  diff domain transform

  Li-Yi Wei
  09/05/2010

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "PlainBoxDomain.hpp"
#include "ToroidalBoxDomain.hpp"

#include "SampledJacobianField.hpp"
#include "ImportanceJacobianField.hpp"

#include "UniformDiffGather.hpp"
#include "SampledIsotropicDiffGather.hpp"
#include "SampledAnisotropicDiffGather.hpp"
#include "UniformLpDiffGather.hpp"

#include "GaussianDiffKernel.hpp"
#include "CosDiffKernel.hpp"
#include "DiffScatter.hpp"

#include "Math.hpp"
#include "Utility.hpp"
#include "FrameBuffer.hpp"
#include "Exception.hpp"
#include "Timer.hpp"

int Main(int argc, char **argv)
{
#if defined(_DIFF_SCATTER)
    int min_argc = 11;
#elif defined(_DIFF_RHO)
    int min_argc = 5;
#else
# error unknown app option
#endif

#if defined(_UNIFORM_DOMAIN)
#elif defined(_ISOTROPIC_DOMAIN)
    min_argc += 8;
#elif defined(_ANISOTROPIC_DOMAIN)
    min_argc += 8;
#elif defined(_ANISOISO_DOMAIN)
    min_argc += 9;
#else
#error unknown domain option
#endif

#if defined(_LP)
    min_argc += 5;
#endif

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0];

#if defined(_LP)
        cerr << " lp local_diff_range mean_dist diff_norm (unit, native, symmetry) diff_weight";
#endif

#if defined(_UNIFORM_DOMAIN)
#elif defined(_ISOTROPIC_DOMAIN)
        cerr << " importance_file_name importance_cell_size isotropic_average_distance max_relative_dd local_diff_range diff_norm (unit, native, symmetry) diff_weight max_cache_size";
#elif defined(_ANISOISO_DOMAIN)
        cerr << " importance_file_name max_importance importance_cell_size isotropic_average_distance max_relative_dd local_diff_range diff_norm (unit, native, symmetry) diff_weight max_cache_size";
#elif defined(_ANISOTROPIC_DOMAIN)
        cerr << " jacobian_file_name jacobian_cell_size anisotropic_average_distance max_relative_dd local_diff_range diff_norm (unit, native, symmetry) diff_weight max_cache_size";
#endif

        cerr << " input-file-name-list dimension sub_domain_min sub_domain_max (only consider diff inside subdomain)";

#if defined(_DIFF_SCATTER)
        cerr << " diff_domain_max (max corner of the diff domain) kernel_option kernel_size (relative to cell size) keep_dc (count self diff or not) output_resolution (per unit domain) output_file_name" << endl;
#endif

        return 1;
    }

    int arg_ctr = 0;

#if defined(_LP)
    const string lp_string = argv[++arg_ctr];
    const float lp = (lp_string.find("inf") != string::npos ? Math::INF : atof(lp_string.c_str()));
    const float local_diff_range = atof(argv[++arg_ctr]);
    const float mean_dist = atof(argv[++arg_ctr]);
    const string diff_norm_option = argv[++arg_ctr];
    const IsotropicDiffGather::DiffNorm diff_norm = UniformLpDiffGather::TranslateNorm(diff_norm_option);
    const string diff_weight_option = argv[++arg_ctr];
    const IsotropicDiffGather::DiffWeight diff_weight = UniformLpDiffGather::TranslateWeight(diff_weight_option);
#else
#endif

#if defined(_ISOTROPIC_DOMAIN)
    const string importance_file_name = argv[++arg_ctr];
    const float importance_cell_size = atof(argv[++arg_ctr]);
    const float isotropic_average_distance = atof(argv[++arg_ctr]);
    const float max_relative_dd = atof(argv[++arg_ctr]);
    const float local_diff_range = atof(argv[++arg_ctr]);
    const string diff_norm_option = argv[++arg_ctr];
    const IsotropicDiffGather::DiffNorm diff_norm = IsotropicDiffGather::TranslateNorm(diff_norm_option);
    const string diff_weight_option = argv[++arg_ctr];
    const IsotropicDiffGather::DiffWeight diff_weight = IsotropicDiffGather::TranslateWeight(diff_weight_option);
    const int max_cache_size = atoi(argv[++arg_ctr]);
#elif defined(_ANISOISO_DOMAIN)
    const string importance_file_name = argv[++arg_ctr];
    const float max_importance = atof(argv[++arg_ctr]);
    const float importance_cell_size = atof(argv[++arg_ctr]);
    const float isotropic_average_distance = atof(argv[++arg_ctr]);
    const float max_relative_dd = atof(argv[++arg_ctr]);
    const float local_diff_range = atof(argv[++arg_ctr]);
    const string diff_norm_option = argv[++arg_ctr];
    const AnisotropicDiffGather::DiffNorm diff_norm = AnisotropicDiffGather::TranslateNorm(diff_norm_option);
    const string diff_weight_option = argv[++arg_ctr];
    const AnisotropicDiffGather::DiffWeight diff_weight = AnisotropicDiffGather::TranslateWeight(diff_weight_option);
    const int max_cache_size = atoi(argv[++arg_ctr]);
#elif defined(_ANISOTROPIC_DOMAIN)
    const string jacobian_file_name = argv[++arg_ctr];
    const float jacobian_cell_size = atof(argv[++arg_ctr]);
    const float anisotropic_average_distance = atof(argv[++arg_ctr]);
    const float max_relative_dd = atof(argv[++arg_ctr]);
    const float local_diff_range = atof(argv[++arg_ctr]);
    const string diff_norm_option = argv[++arg_ctr];
    const AnisotropicDiffGather::DiffNorm diff_norm = AnisotropicDiffGather::TranslateNorm(diff_norm_option);
    const string diff_weight_option = argv[++arg_ctr];
    const AnisotropicDiffGather::DiffWeight diff_weight = AnisotropicDiffGather::TranslateWeight(diff_weight_option);
    const int max_cache_size = atoi(argv[++arg_ctr]);
#endif

    const string input_file_name_list = argv[++arg_ctr];
    const int dimension = atoi(argv[++arg_ctr]);

#if defined(_DIFF_SCATTER)
    if(argc < min_argc + 3*(dimension-1))
#elif defined(_DIFF_RHO)
    if(argc < min_argc + 2*(dimension-1))
#endif
    {
        cerr << "insufficient number of input arguments" << endl;
        return 1;
    }

    vector<float> sub_domain_min(dimension);
    for(unsigned int i = 0; i < sub_domain_min.size(); i++)
    {
        sub_domain_min[i] = atof(argv[++arg_ctr]);
    }

    vector<float> sub_domain_max(dimension);
    for(unsigned int i = 0; i < sub_domain_max.size(); i++)
    {
        sub_domain_max[i] = atof(argv[++arg_ctr]);
    }

#if defined(_DIFF_SCATTER)
    vector<float> diff_domain_max(dimension);
    for(unsigned int i = 0; i < diff_domain_max.size(); i++)
    {
        diff_domain_max[i] = atof(argv[++arg_ctr]);
    }

    const string kernel_option = argv[++arg_ctr];
    
    const float kernel_size = atof(argv[++arg_ctr]);

    const int keep_dc = atoi(argv[++arg_ctr]);

    const int output_resolution = atoi(argv[++arg_ctr]);

    const string output_file_name = argv[++arg_ctr];

    const float cell_size = 1.0/output_resolution;
#elif defined(_DIFF_RHO) 
    const int keep_dc = 0;
#endif

    // gather
#if defined(_UNIFORM_DOMAIN)

#if defined(_LP)
    UniformLpDiffGather diff_gather(lp, local_diff_range, mean_dist, diff_norm, diff_weight);
#else
    UniformDiffGather diff_gather;
#endif

#elif defined(_ISOTROPIC_DOMAIN)

    Array<float> importance;

    if(! FrameBuffer::ReadF2M(importance_file_name, importance))
    {
        cerr << "cannot read from " << importance_file_name << endl;
        return 1;
    }

    const vector<float> domain_size(SampledDistanceField::DomainSize(importance, importance_cell_size));

    PlainBoxDomain domain(domain_size);

    SampledDistanceField distance_field(domain, importance, importance_cell_size, isotropic_average_distance);

    SampledIsotropicDiffGather diff_gather(distance_field, max_relative_dd, local_diff_range, diff_norm, diff_weight, max_cache_size);

#elif defined(_ANISOISO_DOMAIN)

    Array<float> importance;

    if(! FrameBuffer::ReadF2M(importance_file_name, importance))
    {
        cerr << "cannot read from " << importance_file_name << endl;
        return 1;
    }

    const vector<float> domain_size(SampledDistanceField::DomainSize(importance, importance_cell_size));

    PlainBoxDomain domain(domain_size);

    ImportanceJacobianField jacobian_field(domain, importance, max_importance, importance_cell_size, isotropic_average_distance);    

    SampledAnisotropicDiffGather diff_gather(jacobian_field, max_relative_dd, local_diff_range, diff_norm, diff_weight, max_cache_size);

#elif defined(_ANISOTROPIC_DOMAIN)

    Array<Matrix> jacobians;
    if(! Utility::ReadMatrixArray(jacobian_file_name, jacobians))
    {
        cerr << "cannot read from " << jacobian_file_name << endl;
        return 1;
    }
 
    const vector<float> domain_size(SampledJacobianField::DomainSize(jacobians, jacobian_cell_size));

    PlainBoxDomain domain(domain_size);

    SampledJacobianField jacobian_field(domain, jacobians, jacobian_cell_size, anisotropic_average_distance);    

    SampledAnisotropicDiffGather diff_gather(jacobian_field, max_relative_dd, local_diff_range, diff_norm, diff_weight, max_cache_size);

#else
#error unknown domain option
#endif

#if defined(_DIFF_SCATTER)
    // kernel
    DiffKernel *p_diff_kernel = 0;
    float query_scale = 0;

    const string gaussian_string("gaussian");
    const string cos_string("cos");

    if(kernel_option == gaussian_string)
    {
        const float radius_in_std = 3.0;
        p_diff_kernel = new GaussianDiffKernel(radius_in_std);

        query_scale = cell_size;
    }
    else if(kernel_option.find(cos_string) == 0)
    {
        const float offset = atof(kernel_option.substr(cos_string.length()).c_str());

        // cerr << "cos offset: " << offset << endl;
        p_diff_kernel = new CosDiffKernel(offset);
        
        query_scale = 1;
    }
    else
    {
        cerr << "unknown kernel option" << endl;
        return 1;
    }

    if(! p_diff_kernel) throw Exception("! p_diff_kernel");

    DiffKernel & diff_kernel = *p_diff_kernel;
    diff_kernel.SetSize(kernel_size*cell_size);

    // diff scatter
    DiffScatter diff_scatter;

    // init output
    vector<int> output_size(dimension);
    for(unsigned int i = 0; i < output_size.size(); i++)
    {
        output_size[i] = static_cast<int>(output_resolution*diff_domain_max[i]*2);
    }

    Array<float> output(output_size);
    output.Clear(0);
#endif // defined(_DIFF_SCATTER)

    // input samples
    ifstream input_list(input_file_name_list.c_str());

    vector<string> input_file_names;

    if(!input_list)
    {
        cerr << "cannot read from " << input_file_name_list << endl;
        return 1;
    }   
    else
    {
        while(input_list.good())
        {
            string foo;
            input_list >> foo;
            if(foo.length() > 0)
            {
                input_file_names.push_back(foo);
            }
        }
    }

    Timer timer;
    double compute_time = 0;

    double total_num_diffs = 0;
    
#if defined(_DIFF_RHO)
    float min_norm2 = Math::INF;
    int min_norm2_num_samples = 0;
#endif

    vector<DiffGather::Diff> diffs;

    for(unsigned which_input = 0; which_input < input_file_names.size(); which_input++)
    {
        const string input_file_name = input_file_names[which_input];

        // get input samples
        vector<Sample> samples;
    
        if(! Utility::ReadSamples(dimension, input_file_name, samples))
        {
            cerr << "cannot read from " << input_file_name;
            return 1;
        }

        //cerr << "total " << samples.size() << " samples read" << endl;

        if(!timer.Start()) throw Exception("cannot start timer");

        diff_gather.Set(samples);

        // accumulate
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            const bool inside = Utility::Inside(sub_domain_min, sub_domain_max, samples[i]);

            if(inside)
            {
                string message = diff_gather.Get(samples[i], keep_dc, diffs);

                if(message != "")
                {
                    cerr << "error in diff gather: " << message << endl;
                    return 1;
                }

#if defined(_DIFF_SCATTER)

                DiffScatter::Answer answer = diff_scatter.Run(diffs, diff_kernel, cell_size, query_scale, output);

                if(answer.message != "")
                {
                    cerr << "error in diff scatter: " << answer.message << endl;
                    return 1;
                }

                total_num_diffs += answer.count; // diffs.size();
#endif // #if defined(_DIFF_SCATTER)

#if defined(_DIFF_RHO)

                // find the minimum spacing here

                for(unsigned int i = 0; i < diffs.size(); i++)
                {
                    const float norm2 = Utility::Norm2(diffs[i].diff);

                    if(norm2 < min_norm2)
                    {
                        min_norm2 = norm2;
                        min_norm2_num_samples = samples.size();
                    }
                }
#endif // defined(_DIFF_RHO)
            }
        }
         
        if(!timer.Stop()) throw Exception("cannot stop timer");

        compute_time += timer.ElapsedTime();
    }

#if defined(_DIFF_RHO)

    float domain_volume = 1;
#if defined(_UNIFORM_DOMAIN)
#else
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_volume *= domain_size[i];
    }
#endif

    const float max_min_dist = Math::ComputeMaxMinDistance(dimension, min_norm2_num_samples/domain_volume);

    const float min_dist = sqrt(min_norm2);
    cout << "min dist: " << min_dist; 
#if defined(_UNIFORM_DOMAIN)
#else
    cout << ", rho: " << min_dist/max_min_dist;
#endif
    cout << endl;
#endif

    // timing info
    cout << "compute time: " << compute_time << endl;

    // normalization

#if defined(_DIFF_SCATTER)
    float normalization_factor = 1.0; //total_num_diffs;

    // divide by total sum
    float total_sum = 0;
    {
        vector<float> output_values;
        output.Get(output_values);

        for(unsigned int i = 0; i < output_values.size(); i++)
        {
            total_sum += output_values[i];
        }
    }

    // multiply by output size
    float total_volume = 1.0;
    for(unsigned int i = 0; i < output_size.size(); i++)
    {
        total_volume *= output_size[i];
    }

    cerr << "total_num_diffs: " << total_num_diffs << ", total_sum: " << total_sum << ", total_volume: " << total_volume << endl;

    normalization_factor *= total_volume/total_sum;

    {
        vector<float> output_values;
        output.Get(output_values);

        for(unsigned int i = 0; i < output_values.size(); i++)
        {
            output_values[i] *= normalization_factor;
        }

        output.Put(output_values);
    }

    // output
    if(! FrameBuffer::WriteF2M(output, output_file_name))
    {
        cerr << "cannot write to " << output_file_name << endl;
        return 1;
    }

    // clean up
    if(p_diff_kernel)
    {
        delete p_diff_kernel;
        p_diff_kernel = 0;
    }
#endif // defined(_DIFF_SCATTER)

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
