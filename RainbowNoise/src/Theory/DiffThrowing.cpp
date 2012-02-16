/*
  DiffThrowing.cpp
  
  analogous to DartThrowing, 
  but produces coordinate difference pairs instead of real samples

  Li-Yi Wei
  10/30/2008

*/

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Random.hpp"
#include "Exception.hpp"
#include "HatDiffGenerator.hpp"
#include "NeedleDiffGenerator.hpp"
#include "HybridDiffGenerator.hpp"

#include "SampleMineField.hpp"
#include "RMatrix.hpp"
#include "UniformConflictChecker.hpp"

vector<float> GetVector(int argc, char ** argv, const int dimension, int & argCtr)
{
    vector<float> result;
    while(((argCtr+1) < argc) && (result.size() < dimension))
    {
        result.push_back(atof(argv[++argCtr]));
    }

    if(result.size() != dimension)
    {
        throw Exception("result.size() != dimension");
    }

    return result;
}

void Output(const vector<Sample> & differences)
{
    // output stuff
    for(unsigned int i = 0; i < differences.size(); i++)
    {
        for(int j = 0; j < differences[i].coordinate.Dimension(); j++)
        {
            cout << differences[i].coordinate[j] << " ";
        }

        cout << endl;
    }
}

int Produce(const DiffGenerator & diff_generator, const int num_pairs)
{
    vector<Sample> differences(num_pairs);

    if(!diff_generator.Produce(differences))
    {
        cerr << "error in producing differences" << endl;
        return 1;
    }

    // output stuff
    Output(differences);

    // done
    return 0;
}

int Hat(int argc, char **argv)
{
    // input arguments
    int min_argc = 4;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension num_pairs domain_size (dimension floats)" << endl;

        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int num_pairs = atoi(argv[++argCtr]);

    const vector<float> domain_size_spec = GetVector(argc, argv, dimension, argCtr);

    // generation
    HatDiffGenerator diff_generator(domain_size_spec);

    return Produce(diff_generator, num_pairs);
}

int Needle(int argc, char **argv)
{
    // input arguments
    int min_argc = 6;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension needle_dimension needle_length num_pairs domain_size (dimension floats)" << endl;

        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int needle_dimension = atoi(argv[++argCtr]);
    const float needle_length = atof(argv[++argCtr]);
    const int num_pairs = atoi(argv[++argCtr]);

    const vector<float> domain_size_spec = GetVector(argc, argv, dimension, argCtr);

    // generation
    NeedleDiffGenerator diff_generator(domain_size_spec, needle_dimension, needle_length);

    return Produce(diff_generator, num_pairs);
}

int Hybrid(int argc, char **argv)
{
    // input arguments
    int min_argc = 6;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension needle_length needle_variance num_pairs domain_size (dimension floats)" << endl;

        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float needle_length = atof(argv[++argCtr]);
    const float needle_variance = atof(argv[++argCtr]);
    const int num_pairs = atoi(argv[++argCtr]);

    const vector<float> domain_size_spec = GetVector(argc, argv, dimension, argCtr);

    // generation
    HybridDiffGenerator diff_generator(domain_size_spec, needle_length, needle_variance);

    return Produce(diff_generator, num_pairs);
}

int Mine(int argc, char **argv)
{
    // input arguments
    int min_argc = 7;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " sample_file_name dimension num_classes r_values (num_classes floats) num_pairs domain_size (dimension floats)" << endl;

        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const int num_classes = atoi(argv[++argCtr]);
    const vector<float> r_values = GetVector(argc, argv, num_classes, argCtr);
    if(r_values.size() <= 0) throw Exception("r_values.size() <= 0");

    const int num_pairs = atoi(argv[++argCtr]);
    const vector<float> domain_size_spec = GetVector(argc, argv, dimension, argCtr);

    // read in samples
    vector<Sample> samples;
    ifstream input(input_file_name);
if(!input)
    {
        cerr << "cannot read from " << input_file_name;
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
                samples.push_back(sample);
            }
        }
    }

    cerr << "total " << samples.size() << " samples read" << endl;

    // build rmatrix and conflict checker
    const Array<float> r_matrix = RMatrix::BuildRMatrix(dimension, num_classes, RMatrix::OPTIMAL, r_values);
    UniformConflictChecker conflict_checker(r_matrix);

    // build grid
    vector<float> all_r_values;
    r_matrix.Get(all_r_values);
    if(all_r_values.size() <= 0) throw Exception("all_r_values.size() <= 0");
    float min_r_value = all_r_values[0];
    for(unsigned int i = 0; i < all_r_values.size(); i++)
    {
        if(min_r_value > all_r_values[i]) min_r_value = all_r_values[i];
    }

    const Grid::DomainSpec grid_domain_spec = Grid::BuildDomainSpec(domain_size_spec);

    Grid grid(grid_domain_spec, min_r_value);

    // add samples
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        if(!grid.Add(samples[i])) throw Exception("cannot add sample");
    }

    // minefield
    SampleMineField mine_field(grid, conflict_checker);
    
    // generation
    HatDiffGenerator diff_generator(domain_size_spec, mine_field);

    return Produce(diff_generator, num_pairs);
}

int main(int argc, char **argv)
{
    try
    {
        if(argc < 2)
        {
            cerr << "Usage: " << argv[0] << " option" << endl;
            return 1;
        }

        int argCtr = 0;
        const string option = argv[++argCtr];

        // common operation
        Random::InitRandomNumberGenerator();

        if(option == "hat")
        {
            return Hat(argc-1, &argv[1]);
        }
        else if(option == "needle")
        {
            return Needle(argc-1, &argv[1]);
        }
        else if(option == "hybrid")
        {
            return Hybrid(argc-1, &argv[1]);
        }
        else if(option == "mine")
        {
            return Mine(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
