/*
  DartThrowing.cpp

  Li-Yi Wei
  10/18/2008

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "PlainBoxDomain.hpp"
#include "ToroidalBoxDomain.hpp"

#include "GridSampleHolder.hpp"

#include "SampledDistanceField.hpp"

#include "SampledJacobianField.hpp"
#include "ImportanceJacobianField.hpp"

#include "UniformConflictChecker.hpp"
#include "IsotropicConflictChecker.hpp"
#include "AnisotropicConflictChecker.hpp"
#include "UniformLpConflictChecker.hpp"

#include "UniformWhiteNoise.hpp"
#include "IsotropicWhiteNoise.hpp"
#include "AnisotropicWhiteNoise.hpp"

#include "Random.hpp"
#include "Math.hpp"
#include "Utility.hpp"
#include "FrameBuffer.hpp"
#include "Exception.hpp"

// anisoiso: use isotropic white noise source for anisotropic dart throwing
typedef enum {NONE, UNIFORM, ISOTROPIC, ANISOTROPIC, ANISOISO} DomainType;

DomainType DecodeDomainType(const string & option)
{
    DomainType type;

    if(option.find("uniform") != string::npos)
    {
        type = UNIFORM;
    }
    else if(option.find("anisotropic") != string::npos)
    {
        type = ANISOTROPIC;
    }
    else if((option.find("anisoiso") != string::npos) || (option.find("isoaniso") != string::npos))
    {
        type = ANISOISO;
    }
    else if(option.find("isotropic") != string::npos)
    {
        type = ISOTROPIC;
    }

    return type;
}

float DecodeLp(const string & option)
{
    float lp = 2;

    int pos = -1;
    for(unsigned int i = 1; i < option.size(); i++)
    {
        if((option[i-1] == 'l') && ((option[i] == 'i') || ((option[i] >= '0') && (option[i] <= '9'))))
        {
            pos = i;
            break;
        }
    }

    if(pos >= 0)
    {
        if(option[pos] == 'i')
        {
            lp = Math::INF;
        }
        else
        {
            lp = atof(option.substr(pos).c_str());
        }
    }

    return lp;
}

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 8;
    const int isotropic_extra_min_argc = 2;
    const int anisoiso_extra_min_argc = 3;
    const int anisotropic_extra_min_argc = 3;

    if(argc < min_argc)
    {
        cerr << argv[0] << " option (uniform, isotropic/adaptive, anisoiso, anisotropic, uniformlp) dimension r_value k_rho_value domain_size (dimension numbers) boundary (none or toroidal) sample_grid_resolution_per_unit_domain [isotropic: importance_file_name importance_cell_size] [anisoiso: importance_file_name max_importance importance_cell_size] [anisotropic: jacobian_file_name jacobian_cell_size aniso_white_noise_radius] [output_file_name] [init_file_name]" << endl;

        return 1;
    }
    
    int arg_ctr = 0;
    const string option = argv[++arg_ctr];
    
    const DomainType domain_type = DecodeDomainType(option);

    if(domain_type == UNIFORM)
    {
        // no extra min argc
    }
    else if(domain_type == ISOTROPIC)
    {
        min_argc += isotropic_extra_min_argc;
    }
    else if(domain_type == ANISOTROPIC)
    {
        min_argc += anisotropic_extra_min_argc;
    }
    else if(domain_type == ANISOISO)
    {
        min_argc += anisoiso_extra_min_argc;
    }
    else
    {
        cerr << "unknown domain type" << endl;
        return 1;
    }

    const float lp = DecodeLp(option);

    const int dimension = atoi(argv[++arg_ctr]);
    
    const float r_value = atof(argv[++arg_ctr]);

    const float k_rho_number = atof(argv[++arg_ctr]);
    const float rho_number = k_rho_number - floor(k_rho_number);
#if 1
    const int k_number = static_cast<int>(floor(k_rho_number)); // maximum number of trials per successful sample
#else
    const int k_number = (k_rho_number == floor(k_rho_number) ? static_cast<int>(k_rho_number) : 0);
#endif

    if(argc < (min_argc + dimension -1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }

    const int boundary_condition = atoi(argv[++arg_ctr]);

    const int sample_grid_resolution_per_unit_domain = atoi(argv[++arg_ctr]);

    // domain
    Domain * p_domain = 0;
    if(boundary_condition == 0)
    {
        p_domain = new PlainBoxDomain(domain_size);
    }
    else
    {
        p_domain = new ToroidalBoxDomain(domain_size);
    }

    if(!p_domain) throw Exception("null domain");
    const Domain & domain = *p_domain;

    // sample holder
    vector<int> sample_grid_resolution(dimension);
    for(unsigned int i = 0; i < sample_grid_resolution.size(); i++)
    {
        sample_grid_resolution[i] = static_cast<int>(ceil(sample_grid_resolution_per_unit_domain*domain_size[i]));
    }
    const float sample_grid_cell_size = 1.0/sample_grid_resolution_per_unit_domain;

    GridSampleHolder sample_holder(domain, sample_grid_resolution, sample_grid_cell_size);

    // conflict checker
    ConflictChecker * p_conflict_checker = 0;

    Array<float> importance;
    DistanceField * p_distance_field = 0;
    
    JacobianField * p_jacobian_field = 0;

    if(domain_type == UNIFORM)
    {
        if(lp == 2)
        {
            p_conflict_checker = new UniformConflictChecker(r_value);
        }
        else
        {
            p_conflict_checker = new UniformLpConflictChecker(lp, r_value);
        }
    }
    else if(domain_type == ISOTROPIC)
    {
        const string importance_file_name = argv[++arg_ctr];
        const float importance_cell_size = atof(argv[++arg_ctr]);

        if(! FrameBuffer::ReadF2M(importance_file_name, importance))
        {
            cerr << "cannot read from " << importance_file_name << endl;
            return 1;
        }
        
        p_distance_field = new SampledDistanceField(domain, importance, importance_cell_size, r_value);

        if(! p_distance_field) throw Exception("null distance field!");

        p_conflict_checker = new IsotropicConflictChecker(*p_distance_field);
    }
    else if(domain_type == ANISOTROPIC)
    {
        const string jacobian_file_name = argv[++arg_ctr]; // warp matrix name for now

        const float jacobian_cell_size = atof(argv[++arg_ctr]);

        Array<Matrix> jacobians;
        if(! Utility::ReadMatrixArray(jacobian_file_name, jacobians))
        {
            cerr << "cannot read matrix array from " << jacobian_file_name << endl;
            return 1;
        }

        p_jacobian_field = new SampledJacobianField(domain, jacobians, jacobian_cell_size, r_value); 

        if(! p_jacobian_field) throw Exception("null Jacobian field");

        p_conflict_checker = new AnisotropicConflictChecker(*p_jacobian_field);
    }
    else if(domain_type == ANISOISO)
    {
        const string importance_file_name = argv[++arg_ctr];
        const float max_importance = atof(argv[++arg_ctr]);
        const float importance_cell_size = atof(argv[++arg_ctr]);

        if(! FrameBuffer::ReadF2M(importance_file_name, importance))
        {
            cerr << "cannot read from " << importance_file_name << endl;
            return 1;
        }

        p_jacobian_field = new ImportanceJacobianField(domain, importance, max_importance, importance_cell_size, r_value); 

        if(! p_jacobian_field) throw Exception("null Jacobian field");

        p_conflict_checker = new AnisotropicConflictChecker(*p_jacobian_field);
    }
    else
    {
        cerr << "unknown domain type" << endl;
        return 1;
    }

    if(!p_conflict_checker) throw Exception("null conflict checker");

    const ConflictChecker & conflict_checker = *p_conflict_checker;

    // random source
    WhiteNoise *p_white_noise = 0;
    if(domain_type == UNIFORM)
    {
        p_white_noise = new UniformWhiteNoise(domain_size);
    }
    else if((domain_type == ISOTROPIC) || (domain_type == ANISOISO))
    {
        p_white_noise = new IsotropicWhiteNoise(domain_size, importance);
    }
    else if(domain_type == ANISOTROPIC)
    {
        const float anisotropic_white_radius = atof(argv[++arg_ctr]);
        p_white_noise = new AnisotropicWhiteNoise(*p_jacobian_field, anisotropic_white_radius);
    }
    else
    {
        cerr << "unknown domain type" << endl;
        return 1;
    }

    if(! p_white_noise) throw Exception("null white noise");
    
    const WhiteNoise & white_noise = *p_white_noise;

    // predict the target number of samples
    int target_num_samples = ((rho_number > 0) ? Math::ComputeMaxNumSamples(dimension, r_value/rho_number) : static_cast<int>(fabs(k_number*1.0)));
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        target_num_samples *= domain_size[i];
    }

#if 1
    const float max_num_trials = ((rho_number <= 0 || k_number <= 0) ? Math::INF : k_number*target_num_samples);
#else
    int total_num_grid_cells = 1;
    for(unsigned int i = 0; i < sample_grid_resolution.size(); i++)
    {
        total_num_grid_cells *= sample_grid_resolution[i];
    }
    const float max_num_trials = ((rho_number > 0) ? Math::INF : k_number*total_num_grid_cells);
#endif

    // output and init file names
    string output_file_name;

    if((arg_ctr + 1) < argc)
    {
        output_file_name = argv[++arg_ctr];
    }
    else
    {
        output_file_name = "cout";
    }

    string init_file_name;

    if((arg_ctr + 1) < argc)
    {
        init_file_name = argv[++arg_ctr];
    }
    else
    {
        init_file_name = "";
    }

    // init samples
    int num_samples = 0;
    if(init_file_name != "")
    {
        vector<Sample> init_samples;

        if(!Utility::ReadSamples(dimension, init_file_name, init_samples))
        {
            cerr << "cannot read init samples" << endl;
            return 1;
        }

        for(unsigned int i = 0; i < init_samples.size(); i++)
        {
            Sample * p_sample = new Sample(init_samples[i]);
            Sample & sample = *p_sample; 

            if(domain.Inside(sample))
            {
                if(! sample_holder.Add(sample)) throw Exception("cannot add sample");
                num_samples++;
            }
            else
            {
                delete p_sample;
                p_sample = 0;
            }
        }

        cerr << "num init samples: " << num_samples << endl;
    }

    // init random
    Random::InitRandomNumberGenerator();

    // generate samples
#if 0
    cerr << "target_num_samples: " << target_num_samples << ", max_num_trials: " << max_num_trials << endl;
    if(p_jacobian_field)
    {
        cerr << "forward mean: " << p_jacobian_field->ForwardMeanEigenValue() << ", forward max: " << p_jacobian_field->ForwardMaxEigenValue() << endl;
    }
#endif

    int num_trials = 0;
    while((num_samples < target_num_samples) && (num_trials < max_num_trials))
    {
#if 0
        cerr << "num_samples: " << num_samples << ", num_trials: " << num_trials << endl;
#endif

        Sample * p_sample = new Sample(dimension);
        Sample & sample = *p_sample;

        // white noise sample
        do
        {
            if(! white_noise.Get(sample)) throw Exception("cannot generate white noise");
        }
        while(! domain.Inside(sample));

        // conflict check
        const ConflictChecker::Status status = conflict_checker.Check(sample, sample_holder);

        if(status == ConflictChecker::CHECK_PASS)
        {
            if(! sample_holder.Add(sample)) throw Exception("cannot add sample");
            num_samples++;
        }
        else if(status == ConflictChecker::CHECK_ERROR)
        {
            delete p_sample;
            throw Exception("error in conflit check");
        }
        else
        {
            // ConflictChecker::CHECK_FAIL
            delete p_sample;
        }

        num_trials++;
    }

    // write out
    vector<const Sample *> p_samples;
    sample_holder.GetSamples(p_samples);

    vector<Sample> output_samples(p_samples.size());
    for(unsigned int i = 0; i < output_samples.size(); i++)
    {
        output_samples[i] = *p_samples[i];
    }

    if(! Utility::WriteSamples(output_samples, output_file_name))
    {
        cerr << "cannot write samples to " << output_file_name << endl;
        return 1;
    }

    // clean up
    if(p_jacobian_field)
    {
        delete p_jacobian_field;
        p_jacobian_field = 0;
    }

    if(p_distance_field)
    {
        delete p_distance_field;
        p_distance_field = 0;
    }

    for(unsigned int i = 0; i < p_samples.size(); i++)
    {
        if(!p_samples[i]) delete p_samples[i];
    }
    p_samples.clear();

    if(p_white_noise)
    {
        delete p_white_noise; p_white_noise = 0;
    }

    if(p_conflict_checker)
    {
        delete p_conflict_checker; p_conflict_checker = 0;
    }

    if(p_domain)
    {
        delete p_domain; p_domain = 0;
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
