/*
  DiscreteRelaxSamples.cpp

  Li-Yi Wei
  05/16/2008

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Exception.hpp"
#include "LloydRelaxation.hpp"
#include "DiscreteRelaxation.hpp"

#define _USE_LLOYD_RELAXATION

float Distance(const Sample & s1, const Sample & s2)
{
    if(s1.coordinate.Dimension() != s2.coordinate.Dimension())
    {
        throw Exception("s1.coordinate.Dimension() != s2.coordinate.Dimension()");
    }

    float distance = 0;
    for(int i = 0; i < s1.coordinate.Dimension(); i++)
    {
        const float foo = s1.coordinate[i] - s2.coordinate[i];
        distance += foo*foo;
    }

    return distance;
}

int Main(int argc, char **argv)
{
    const int num_fixed_arguments = 8;
    if(argc < num_fixed_arguments)
    {
        cerr << "Usage: " << argv[0] << " input-file-name dimension domain_size  num_continuous_iterations num_discrete_iterations sample_group_id measure_energy" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);

#ifdef _USE_LLOYD_RELAXATION
    if(dimension != 2)
    {
        cerr << "dimension has to be 2" << endl;
        return 1;
    }
#endif

    if(argc < (num_fixed_arguments + dimension -1))
    {
        cerr << "not enough arguments" << endl;
        return 1;
    }
    Coordinate corner_min(dimension);
    Coordinate corner_max(dimension);
    for(int i = 0; i < dimension; i++)
    {
        corner_min[i] = 0;
        corner_max[i] = atof(argv[++argCtr]);
    }
    const DiscreteRelaxation::DomainSpec domain_spec(corner_min, corner_max);
    const int num_continuous_iterations = atoi(argv[++argCtr]);
    const int num_discrete_iterations = atoi(argv[++argCtr]);
    const int sample_group_id = atoi(argv[++argCtr]);
    const int measure_energy = atoi(argv[++argCtr]);
    const DiscreteRelaxation::BoundaryCondition boundary_condition = DiscreteRelaxation::TOROIDAL;

    // get input samples
    ifstream input(input_file_name); // read in samples
    // cerr << "read in samples " << endl;
    
    vector<Sample> samples;
    
    if(!input)
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
                input >> sample.coordinate[i];
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    cerr << "total " << samples.size() << " samples read" << endl;

    // check for 2 class case
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if((samples[i].id < 0) || (samples[i].id > 1))
        {
            cerr << "can only handle 2 classes now" << endl;
            return 1;
        }
    }
    if((sample_group_id < 0) || (sample_group_id > 1))
    {
        cerr << "can only handle 2 classes now" << endl;
        return 1;
    }

    // process
    vector<Sample> representatives;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(samples[i].id == sample_group_id)
        {
            representatives.push_back(samples[i]);
        }
    }

    cerr << representatives.size() << " input representatives" << endl;

#ifdef _USE_LLOYD_RELAXATION
    vector<LloydRelaxation::Point2D> points(samples.size());
    vector<const LloydRelaxation::Point2D *> p_points(samples.size());
    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i].x = samples[i].coordinate[0];
        points[i].y = samples[i].coordinate[1];
        p_points[i] = &points[i];
    }
    vector<LloydRelaxation::Point2D> representative_points(representatives.size());
    vector<LloydRelaxation::Point2D *> p_representative_points(representatives.size());
    for(unsigned int i = 0; i < representative_points.size(); i++)
    {
        representative_points[i].x = representatives[i].coordinate[0]; 
        representative_points[i].y = representatives[i].coordinate[1];
        p_representative_points[i] = &representative_points[i];
    }
#endif

    const int num_iterations = num_continuous_iterations*num_discrete_iterations;
    for(int which_iteration = 0; which_iteration < num_iterations; which_iteration++)
    {
#ifdef _USE_LLOYD_RELAXATION
        string message = LloydRelaxation::RunOnce(domain_spec.corner_min[0], domain_spec.corner_max[0], domain_spec.corner_min[1], domain_spec.corner_max[1], static_cast<LloydRelaxation::BoundaryCondition>(boundary_condition), p_representative_points);
        if(message != "")
        {
            cerr << "error in LloydRelaxation: " << message << endl;
            return 1;
        }

        if((which_iteration+1)%num_continuous_iterations == 0)
        {
            message = LloydRelaxation::Snap(domain_spec.corner_min[0], domain_spec.corner_max[0], domain_spec.corner_min[1], domain_spec.corner_max[1], static_cast<LloydRelaxation::BoundaryCondition>(boundary_condition), p_representative_points, p_points);
            if(message != "")
            {
                cerr << "error in Snap: " << message << endl;
                return 1;
            }
        }       
#else
        const string message = DiscreteRelaxation::RunOnce(domain_spec, boundary_condition, samples, representatives);
        if(message != "")
        {
            cerr << "error in DiscreteRelaxation: " << message << endl;
            return 1;
        }
#endif

#ifdef _USE_LLOYD_RELAXATION
        for(unsigned int i = 0; i < representatives.size(); i++)
        {
            representatives[i].coordinate[0] = representative_points[i].x;
            representatives[i].coordinate[1] = representative_points[i].y;
        }
#endif
        if(measure_energy)
        {
            cerr << which_iteration << " ";
            cerr << ": energy "<< DiscreteRelaxation::Energy(domain_spec, boundary_condition, samples, representatives);
            cerr << endl;
        }
    }

    cerr << representatives.size() << " output representatives" << endl;

    // process ids, assuming only 2 classes
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        samples[i].id = 1 - sample_group_id;
    }
    for(unsigned int i = 0; i < representatives.size(); i++)
    {
        // find myself
        for(unsigned int j = 0; j < samples.size(); j++)
        {
            if(Distance(representatives[i], samples[j]) <= 0)
            {
                samples[j].id = sample_group_id;
            }
        }
    }

#if 1
    // output
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        cout << samples[i].id << " ";
        for(int k = 0; k < samples[i].coordinate.Dimension(); k++)
        {
            cout << samples[i].coordinate[k] << " ";
        }
        cout << endl;
    }
#else // debug
    Sample foo(samples[0]);
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        const int my_slot = DiscreteRelaxation::Nearest(domain_spec, boundary_condition, representatives, samples[i], foo);
        samples[i].id = my_slot;

        cout << samples[i].id + sample_group_id + 1 << " ";
        for(int k = 0; k < samples[i].coordinate.Dimension(); k++)
        {
            cout << samples[i].coordinate[k] << " ";
        }
        cout << endl;
    }
    for(unsigned int i = 0; i < representatives.size(); i++)
    {
        cout << sample_group_id << " ";
        for(int k = 0; k < representatives[i].coordinate.Dimension(); k++)
        {
            cout << representatives[i].coordinate[k] << " ";
        }
        cout << endl;
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
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
