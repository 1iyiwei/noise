/*
  DiscreteDartThrowing.cpp

  brute force dart throwing for multi-class samples
  but constrained within an existing set of samples

  Li-Yi Wei
  05/25/2008

*/

#if 1

#define _DISCRETE_DOMAIN
#include "DartThrowing.cpp"

#else

#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Exception.hpp"
#include "Grid.hpp"
#include "UniformConflictChecker.hpp"
#include "Random.hpp"
#include "RMatrix.hpp"
#include "Math.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 8;
    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension num_classes priority (c numbers with low values indicating higher priority) r_values (c*(c+1)/2 numbers in row major order of the upper matrix, or only c diagonal entries) k_number (positive for the usual k number, negative for target number of samples) domain_size (dimension numbers) input_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    if(dimension <= 0)
    {
        cerr << "dimension must be > 0" << endl;
        return 1;
    }
    const int num_classes = atoi(argv[++argCtr]);
    if(num_classes <= 0)
    {
        cerr << "num_classes must be > 0" << endl;
        return 1;
    }

    vector<int> priority_values(num_classes);
    for(unsigned int i = 0; i < priority_values.size(); i++)
    {
        priority_values[i] = atoi(argv[++argCtr]);

        if((priority_values[i] < 0) || (priority_values[i] >= num_classes))
        {
            cerr << "priority number must be within [0 num_classes-1]" << endl;
            return 1;
        }
    }

    min_argc += (dimension-1)+(num_classes-1); // we now know the # of domain_size arguments and # of priority arguments

    const int num_r_values = argc - min_argc + 1;

    if((num_r_values != num_classes) && (num_r_values != num_classes*(num_classes+1)/2))
    {
        cerr << "wrong number of r_values";
        return 1;
    }

    vector<float> input_r_values;
    for(int i = 0; i < num_r_values; i++)
    {
        input_r_values.push_back(atof(argv[++argCtr]));
    }

    const Array<float> r_matrix = RMatrix::BuildRMatrix(dimension, num_classes, RMatrix::OPTIMAL, input_r_values);

    vector<float> r_values; r_matrix.Get(r_values);

    if(0) // debug
    {
        cerr << "r_values: ";
        for(unsigned int i = 0; i < r_values.size(); i++)
        {
            cerr << r_values[i] << " ";
        }
        cerr << endl;
    }

    const int k_number = atoi(argv[++argCtr]);

    vector<float> domain_size_spec;
    while(((argCtr+1) < argc) && (domain_size_spec.size() < dimension))
    {
        domain_size_spec.push_back(atof(argv[++argCtr]));
    }

    if(domain_size_spec.size() != dimension)
    {
        // should probably use assert...
        throw Exception("domain_size_spec.size() != dimension");
    }

    for(unsigned int i = 0; i < domain_size_spec.size(); i++)
    {
        if(domain_size_spec[i] <= 0)
        {
            cerr << "domain_size_spec[" << i << "] <= 0" << endl;
            return 1;
        }
    }

    const char * input_file_name = argv[++argCtr];

    ifstream input(input_file_name);
    vector<Sample> input_samples;
    if(! input)
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
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
                input_samples.push_back(sample);
            }
        }
    }

    // conflict checker    
    UniformConflictChecker conflict_checker(r_matrix);

    // init grid
    const Grid::DomainSpec grid_domain_spec = Grid::BuildDomainSpec(domain_size_spec);

    float min_r_value = r_values[0];
    float max_r_value = r_values[0];
    for(unsigned int i = 0; i < r_values.size(); i++)
    {
        if(r_values[i] < min_r_value) min_r_value = r_values[i];
        if(r_values[i] > max_r_value) max_r_value = r_values[i];
    }
    Grid grid(grid_domain_spec, min_r_value);

    // cerr << "min_r_value: " << min_r_value << endl;

    // play
    const vector<int> num_grid_cells = grid.NumCells();
    int num_trials = k_number;
    for(unsigned int i = 0; i < num_grid_cells.size(); i++)
    {
        num_trials *= num_grid_cells[i];
    }

    const int target_num_samples = -k_number;

    if(k_number >= 0)
    {
        cerr << num_trials << " trials" << endl;
    }
    else
    {
        if(target_num_samples > input_samples.size())
        {
            cerr << "warning: target_num_samples " << target_num_samples << " > input_samples.size() " << input_samples.size() << endl;
        }

        cerr << target_num_samples << " target samples" << endl;
    }

    {
        cerr << "num_grid_cells: ";
        for(unsigned int i = 0; i < num_grid_cells.size(); i++)
        {
            cerr << num_grid_cells[i] << " ";
        }
        cerr << endl;
    }

    vector<PriorityGroup> priority_groups = BuildPriorityGroups(dimension, priority_values, r_matrix, num_trials, target_num_samples);

    Random::InitRandomNumberGenerator(time(0));

    int total_num_samples = 0;
    for(unsigned int which_priority_group = 0; which_priority_group < priority_groups.size(); which_priority_group++)
    {
        const PriorityGroup & priority_group = priority_groups[which_priority_group];

        if(0) // debug
        {
            cerr << "classes: (";
            for(unsigned int i = 0; i < priority_group.classes.size(); i++)
            {
                cerr << priority_group.classes[i] << " ";
            }
            cerr << "), k_number: " << k_number << ", num_trials: " << priority_group.num_trials << ", num_samples: " << priority_group.target_num_samples << endl;
        }

        int num_samples = 0;
        for(int trial = 0; (input_samples.size() > 0) && (((k_number >= 0) && (trial < priority_group.num_trials)) || ((k_number < 0) && (num_samples < priority_group.target_num_samples))); trial++)
        {
            const int selection = static_cast<int>(floor(Random::UniformRandom() * input_samples.size()))%input_samples.size();

            Sample * sample = new Sample(input_samples[selection]);
        
            if(priority_group.classes.size() <= 0)
            {
                throw Exception("priority_group.classes.size() <= 0");
            }

            sample->id = static_cast<int>(floor(Random::UniformRandom()*priority_group.classes.size()))%priority_group.classes.size();
            sample->id = priority_group.classes[sample->id];

            if(sample->coordinate.Dimension() != grid_domain_spec.domain_size.size())
            {
                throw Exception("sample->coordinate.size() != grid_domain_spec.domain_size.size()");
            }

            if(grid.Inside(*sample) && !grid.Conflict(*sample, conflict_checker))
            {
                if(!grid.Add(*sample))
                {
                    throw Exception("cannot add sample");
                    delete sample; sample = 0;
                }
                else
                {
                    num_samples++;
                    total_num_samples++;
                    input_samples[selection] = input_samples[input_samples.size()-1];
                    input_samples.pop_back();
                }
            }
            else
            {
                delete sample; sample = 0;
            }
        }
    }

    // output
    vector<const Sample *> samples;
    grid.GetSamples(samples);

    if(total_num_samples != samples.size())
    {
        throw Exception("total_num_samples != samples.size()");
    }

    cerr << samples.size() << " samples" << endl;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        cout << samples[i]->id << " ";
        for(int j = 0; j < samples[i]->coordinate.Dimension(); j++)
        {
            cout << samples[i]->coordinate[j] << " ";
        }
        cout << endl;
    }

    // clean up
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        delete samples[i];
    }
    samples.clear();

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
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}

#endif
