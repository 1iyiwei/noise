/*
  DartThrowing.cpp

  brute force dart throwing for multi-class samples

  Li-Yi Wei
  04/25/2008

*/

#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "Exception.hpp"
#include "Timer.hpp"

#if defined(_USE_SINGLE_TREE)
#include "Tree.hpp" 
#elif defined(_USE_MULTI_TREE)
#include "MultiTree.hpp"
#endif
#include "Grid.hpp"

#include "UniformDistanceField.hpp"

#if defined(_FUZZY_CONFLICT_METRIC)
#include "FuzzyConflictChecker.hpp"
#elif defined(_ADAPTIVE_SAMPLING)
#include "SampledRMatrixField.hpp"
#include "AdaptiveConflictChecker.hpp"
#else
#include "UniformConflictChecker.hpp"
#endif

#include "Random.hpp"
#include "RMatrix.hpp"
#include "Math.hpp"
#include "Utility.hpp"

#include "SampleRecord.hpp"

// pick one and only one strategy from below
// the old strategy keeps on trying for the most under priviledged class
// prone to hang
// the new strategy would be "soft", 
// throwing a non-classified sample first
// and try to fit class numbers from the most under priviledged
// #define _STUCKY_RHO_STRATEGY

// #define _CLASSLESS_RHO_STRATEGY

#ifndef _STUCKY_RHO_STRATEGY
#ifndef _CLASSLESS_RHO_STRATEGY
// a strategy that allows the removal of existing neighbors
// the best and default option
#define _PSYCHO_RHO_STRATEGY
#endif
#endif

struct Sortee
{
    Sortee(void) : id(0), value(0) {};
    Sortee(const int id_i, const float value_i) : id(id_i), value(value_i) {};

    int operator<(const Sortee & another) const
        {
            return (value < another.value);
        };

    int id;
    float value;
};

int Inferior(const vector<int> & vec1, const vector<int> & vec2, const vector<int> & classes)
{
    int inferior = 0;

    if(vec1.size() != vec2.size())
    {
        throw Exception("vec1.size() != vec2.size()");
    }

    for(unsigned int i = 0; i < classes.size(); i++)
    {
        const int class_id = classes[i];
        if(vec1[class_id] < vec2[class_id]) inferior = 1;
    }

    return inferior;
}

int RandomClass(const vector<float> & cdf)
{
    const float value = Random::UniformRandom()*cdf[cdf.size()-1];
    
    int selection = 0;

    for(unsigned int i = 0; i < cdf.size(); i++)
    {
        if(value > cdf[i])
        {
            selection = i+1;
        }
        else
        {
            break;
        }
    }

    return selection%cdf.size();
}

int Main(int argc, char **argv)
{
    // input arguments
    int min_argc = 7;

#ifdef _DISCRETE_DOMAIN
    min_argc++;
#endif

#ifdef _PATIENT_KILLING
    min_argc++;
#endif

#ifdef _RECORD_SAMPLE_HISTORY
    min_argc++;
#endif

#ifdef _FUZZY_CONFLICT_METRIC
    min_argc++;
#endif

#ifdef _ADAPTIVE_SAMPLING
    min_argc += 2;
#endif

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " dimension num_classes (positive for optimal rmatrix computation, negative for uniform off-diagonal entries) priority (either c integers with low values indicating higher priority, or c floating points indicating class selection probability) r_values (c*(c+1)/2 numbers in row major order of the upper matrix, or only c diagonal entries) k_number (positive integer for the usual k number, negative integer for target number of samples, [0 1] float for rho-number, or positive float for specifying both the k-number/patience-factor and the rho-number) domain_size (dimension numbers)";

#ifdef _DISCRETE_DOMAIN
        cerr << " input_sample_file_name";
#endif

#ifdef _PATIENT_KILLING
        cerr << " patience_factor";
#endif

#ifdef _RECORD_SAMPLE_HISTORY
        cerr<< " history_file_name";
#endif

#ifdef _FUZZY_CONFLICT_METRIC
        cerr<< " monomial_degree";
#endif

#ifdef _ADAPTIVE_SAMPLING
        cerr<< " conflict_metric (MEAN or MAX) importance_file_names (num_classes strings)";
#endif

        cerr << endl;

        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    if(dimension <= 0)
    {
        cerr << "dimension must be > 0" << endl;
        return 1;
    }

    const char * num_classes_spec = argv[++argCtr];
    if(strlen(num_classes_spec) <= 0)
    {
        cerr << "empty specification for num_classes" << endl;
        return 1;
    }

    const int num_classes_i = atoi(num_classes_spec);
    const RMatrix::Method rmatrix_method = (num_classes_spec[0] == '0' ? RMatrix::GEOMETRIC : (num_classes_i > 0 ? RMatrix::OPTIMAL : RMatrix::UNIFORM));
 
    const int num_classes = num_classes_i > 0 ? num_classes_i : -num_classes_i;
    if(num_classes <= 0)
    {
        cerr << "num_classes must be > 0" << endl;
        return 1;
    }

    int class_probability_all_int = 1;
    vector<float> class_probability(num_classes);
    for(unsigned int i = 0; i < class_probability.size(); i++)
    {
        class_probability[i] = atof(argv[++argCtr]);
        if(floor(class_probability[i]) != class_probability[i])
        {
            class_probability_all_int = 0;
        }
    }

    vector<int> priority_values(num_classes);
    for(unsigned int i = 0; i < priority_values.size(); i++)
    {
        priority_values[i] = class_probability_all_int ? static_cast<int>(floor(class_probability[i])) : 0;

        if((priority_values[i] < 0) || (priority_values[i] >= num_classes))
        {
            cerr << "priority number must be within [0 num_classes-1]" << endl;
            return 1;
        }
    }

    if(class_probability_all_int)
    {
        for(unsigned int i = 0; i < class_probability.size(); i++)
        {
            class_probability[i] = (i+1.0)/num_classes;
        }
    }
    else
    { 
        for(unsigned int i = 1; i < class_probability.size(); i++)
        {
            class_probability[i] += class_probability[i-1];
        }
    }

    cerr << "class_probablility cdf: ";
    for(unsigned int i = 0; i < class_probability.size(); i++)
    {
        cerr << class_probability[i] << " ";
    }
    cerr << endl;

    min_argc += (dimension-1)+(num_classes-1); // we now know the # of domain_size arguments and # of priority arguments

#ifdef _ADAPTIVE_SAMPLING
    min_argc += (num_classes-1); // importance field names
#endif

    int num_r_values = argc - min_argc + 1;

#ifdef _RECORD_SAMPLE_HISTORY
    int has_sample_history = 1;
#endif

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

    if(0) // debug
    {
        cerr << "input_r_values: ";
        for(unsigned int i = 0; i < input_r_values.size(); i++)
        {
            cerr << input_r_values[i] << " ";
        }
        cerr << endl;
    }

    const Array<float> r_matrix = RMatrix::BuildRMatrix(dimension, num_classes, rmatrix_method, input_r_values);

    vector<float> r_values; r_matrix.Get(r_values);

    for(unsigned int i = 0; i < r_values.size(); i++)
    {
        if(r_values[i] <= 0)
        {
            cerr << "bad r_matrix" << endl;
            return 1;
        }
    }

    if(0) // debug
    {
        cerr << "r_values: ";
        for(unsigned int i = 0; i < r_values.size(); i++)
        {
            cerr << r_values[i] << " ";
        }
        cerr << endl;
    }

    const float k_rho_number = atof(argv[++argCtr]);
    const float rho_number = k_rho_number - floor(k_rho_number);
    const int k_number = (k_rho_number == floor(k_rho_number) ? static_cast<int>(k_rho_number) : 0);

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
            cerr << "domain_size_spec[" << i << "] <= 0 - " << domain_size_spec[i] << endl;
            return 1;
        }
    }

#ifdef _DISCRETE_DOMAIN
    const char * input_sample_file_name = argv[++argCtr];

    ifstream input(input_sample_file_name);
    vector<Sample> input_samples;
    vector<Sample> fixed_samples;
    if(! input)
    {
        cerr << "cannot read from " << input_sample_file_name << endl;
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
                if(sample.id >= 0)
                {
                    fixed_samples.push_back(sample);
                }
                else
                {
                    input_samples.push_back(sample);
                }
            }
        }
    }

    cerr << "num fixed samples: " << fixed_samples.size() << ", num input samples: " << input_samples.size() << endl;

#endif

#ifdef _PATIENT_KILLING 
    const float patience_factor = atof(argv[++argCtr]);
#else
    const float patience_factor = max(static_cast<double>(1.0), abs(static_cast<double>(floor(k_rho_number))));
#endif

#if defined(_USE_SINGLE_TREE) || defined(_USE_MULTI_TREE)
    if(patience_factor < 1.0)
    {
        cerr << "patience_factor < 1.0 is unlikely to give you correct result for tree structure" << endl;
        return 1;
    }
#endif

    if(patience_factor != 0)
    {
        cerr << "patience_factor: " << patience_factor << endl;
    }

#ifdef _RECORD_SAMPLE_HISTORY
    const char * history_file_name = has_sample_history ? argv[++argCtr] : 0;
    // cerr << "history " << has_sample_history << " " << (has_sample_history ? history_file_name : "none") << endl;
#endif

#ifdef _FUZZY_CONFLICT_METRIC
    const int monomial_degree = atoi(argv[++argCtr]);
#endif

    // distance field
    vector<float> class_r_values(num_classes);
    {
        for(unsigned int i = 0; i < class_r_values.size(); i++)
        {
            if(!r_matrix.Get(vector<int>(2, i), class_r_values[i]))
            {
                throw Exception("cannot get value for class_r_values");
            }
        }
    }

#if defined(_ADAPTIVE_SAMPLING)
    const ConflictChecker::Metric conflict_metric = static_cast<AdaptiveConflictChecker::Metric>(atoi(argv[++argCtr]));
    
    vector<string> importance_file_names;
    while(importance_file_names.size() < num_classes)
    {
        if((argCtr+1) < argc)
        {
            importance_file_names.push_back(argv[++argCtr]);
        }
        else
        {
            cerr << "not enough arguments for importance file names";
            return 1;
        }
    }

    {
        // normalize r values
        if(! Utility::NormalizeRValues(domain_size_spec, importance_file_names, class_r_values, class_r_values)) throw Exception("error in NormalizeRValues");
        Array<float> & my_r_matrix = const_cast< Array<float> & >(r_matrix);
        my_r_matrix = RMatrix::BuildRMatrix(dimension, num_classes, rmatrix_method, class_r_values);
    }
    const SampledDistanceField * sampled_distance_field = Utility::GetDistanceField(domain_size_spec, class_r_values, importance_file_names);
    const DistanceField & distance_field = *sampled_distance_field;
#else
    const UniformDistanceField distance_field(domain_size_spec, class_r_values);
#endif

    // conflict checker  
#if defined(_FUZZY_CONFLICT_METRIC)
    MonomialProfiler profiler(monomial_degree);
    FuzzyConflictChecker conflict_checker(r_matrix, profiler);
#elif defined(_ADAPTIVE_SAMPLING)
    SampledRMatrixField r_matrix_field(*sampled_distance_field);
    AdaptiveConflictChecker conflict_checker(r_matrix_field, conflict_metric);
#else  
    UniformConflictChecker conflict_checker(r_matrix);
#endif

    // init grid
    const Grid::DomainSpec grid_domain_spec = Grid::BuildDomainSpec(domain_size_spec);

    float min_r_value = r_values[0];
    float max_r_value = r_values[0];
    for(unsigned int i = 0; i < r_values.size(); i++)
    {
        if(r_values[i] < min_r_value) min_r_value = r_values[i];
        if(r_values[i] > max_r_value) max_r_value = r_values[i];
    }

    if(min_r_value <= 0)
    {
        cerr << "min_r_value <= 0" << endl;
        return 1;
    }

#if defined(_USE_SINGLE_TREE)
#ifndef _GRID_LIKE_TREE
    Tree tree(distance_field, conflict_checker);
#else
    Grid::DomainSpec hack_domain_spec = grid_domain_spec;
    { 
        Grid grid_local(grid_domain_spec, min_r_value);

        hack_domain_spec.domain_size = grid_local.NumCells();
        hack_domain_spec.cell_size = grid_local.CellSize();
    }
    Tree tree(distance_field, conflict_checker, hack_domain_spec); // debug
#endif
#elif defined(_USE_MULTI_TREE)
    MultiTree multi_tree(distance_field, conflict_checker);
#else
    Grid grid(grid_domain_spec, min_r_value);
#endif

    // cerr << "min_r_value: " << min_r_value << endl;

    // play
    int num_trials = k_number;
    int total_num_grid_cells = 1;
    {
        Grid grid_local(grid_domain_spec, min_r_value);
        const vector<int> num_grid_cells = grid_local.NumCells();
        for(unsigned int i = 0; i < num_grid_cells.size(); i++)
        {
            num_trials *= num_grid_cells[i];
            total_num_grid_cells *= num_grid_cells[i];
        }
        
#ifdef _USE_SINGLE_TREE
#ifdef _GRID_LIKE_TREE // debug, making the tree uniform now
        while(tree.CellSize(tree.NumLevels()-1) > grid_local.CellSize())
        {
            if(!tree.Subdivide(1)) // blind subdivision
            {
                throw Exception("error in blind subdivision");
            }
        }
#endif
#endif

#ifdef _USE_MULTI_TREE
#ifdef _GRID_LIKE_TREE // debug, making the tree uniform now
        for(int i = 0; i < multi_tree.NumTrees(); i++)
        {
            Tree & tree = multi_tree.GetTree(i);
            while(tree.CellSize(tree.NumLevels()-1) > grid_local.CellSize())
            {
                if(!tree.Subdivide(1)) // blind subdivision
                {
                    throw Exception("error in blind subdivision");
                }
            }

            cerr << "tree " << i << " has " << tree.NumLevels() << " levels" << endl;
        }
#endif
#endif

#if 0
        cerr << "num_grid_cells: ";
        for(unsigned int i = 0; i < num_grid_cells.size(); i++)
        {
            cerr << num_grid_cells[i] << " ";
        }
        cerr << endl;
#endif
    }

    int target_num_samples = -k_number;

    vector<int> target_num_samples_per_class(num_classes, 0);
    if((rho_number > 0) && (rho_number < 1))
    {
        cerr << "rho_number " << rho_number << endl;

        for(int i = 0; i < num_classes; i++)
        {
            float value = 0;
            r_matrix.Get(vector<int>(2, i), value);

            target_num_samples_per_class[i] = Math::ComputeMaxNumSamples(dimension, value/rho_number);

            for(unsigned int j = 0; j < domain_size_spec.size(); j++)
            {
                target_num_samples_per_class[i] *= domain_size_spec[j];
            }
        }

#if defined(_DISCRETE_DOMAIN)
        {
            int total_target_num_samples = 0;
            for(unsigned int i = 0; i < target_num_samples_per_class.size(); i++)
            {
                total_target_num_samples += target_num_samples_per_class[i];
            }

            for(unsigned int i = 0; i < target_num_samples_per_class.size(); i++)
            {
                target_num_samples_per_class[i] = static_cast<int>(floor(rho_number*target_num_samples_per_class[i]*(fixed_samples.size() + input_samples.size())/total_target_num_samples + 0.5));
            }
        }
#endif

        cerr << "target number of samples per class :";
        for(unsigned int i = 0; i < target_num_samples_per_class.size(); i++)
        {
            cerr << " " << target_num_samples_per_class[i];
        }
        cerr << endl;

#ifndef _STUCKY_RHO_STRATEGY
        target_num_samples = 0;
        for(unsigned int i = 0; i < target_num_samples_per_class.size(); i++)
        {
            target_num_samples += target_num_samples_per_class[i];
        }
        
        cerr << target_num_samples << " target samples" << endl;
#endif
    }
    else
    {
        if(k_number >= 0)
        {
            cerr << num_trials << " trials" << endl;
        }
        else
        {
            cerr << target_num_samples << " target samples" << endl;
        }
    }

    vector<PriorityGroup> priority_groups = BuildPriorityGroups(dimension, priority_values, r_matrix, num_trials, target_num_samples);

    const unsigned long random_seed = time(0);
    // cerr << "random seed: " << random_seed << endl;
    Random::InitRandomNumberGenerator(random_seed);

    int total_num_samples = 0;
    vector<int> num_samples_per_class(num_classes, 0);
    vector<int> current_failures_per_class(num_classes, 0);

#if defined(_DISCRETE_DOMAIN)
#if defined(_USE_SINGLE_TREE) || defined(_USE_MULTI_TREE)
#error cannot support tree and discrete domain together for now!
#else
    // add fixed samples
    for(unsigned int i = 0; i < fixed_samples.size(); i++)
    {
        Sample * sample = new Sample(fixed_samples[i]);
        if(! grid.Add(*sample)) throw Exception("cannot add fixed sample");
        if((sample->id >= 0) && (sample->id < num_samples_per_class.size()))
        {
            total_num_samples++;
            num_samples_per_class[sample->id]++;
        }
        else
        {
            throw Exception("illegal fixed sample class id");
        }
    }
#endif

#endif

#ifdef _RECORD_SAMPLE_HISTORY
    // record class numbers for each sample drawn
    vector<SampleRecord> sample_history;
#endif

    vector<int> subdividable(num_classes, 1);

    Timer timer;

    timer.Start();

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
        {
            for(unsigned int i = 0; i < priority_group.classes.size(); i++)
            {
                num_samples += num_samples_per_class[priority_group.classes[i]];
            }
        }

#if defined(_STUCKY_RHO_STRATEGY)
        for(int trial = 0; ((k_number > 0) && (trial < priority_group.num_trials)) || ((k_number < 0) && (num_samples < priority_group.target_num_samples)) || ((k_number == 0) && Inferior(num_samples_per_class, target_num_samples_per_class, priority_group.classes)); trial++)
#else
        for(int trial = 0; ((k_number > 0) && (trial < priority_group.num_trials)) || ((k_number <= 0) && (num_samples < priority_group.target_num_samples)); trial++)
#endif
        {
#if defined(_DISCRETE_DOMAIN)
            if(input_samples.size() <= 0) break;
#endif

            Sample * sample = new Sample(dimension);
        
            if(priority_group.classes.size() <= 0)
            {
                throw Exception("priority_group.classes.size() <= 0");
            }

            vector<int> sample_ids;

            if(k_number == 0)
            {
#ifndef _CLASSLESS_RHO_STRATEGY
                // pick the most under filled class
                float min_fill_ratio = 2.0;
                int min_fill_id = -1;

                for(unsigned int i = 0; i < priority_group.classes.size(); i++)
                {
                    const int current_id = priority_group.classes[i];
                    const float fill_ratio = num_samples_per_class[current_id]*1.0/target_num_samples_per_class[current_id];
                    if(min_fill_ratio >= fill_ratio)
                    {
                        min_fill_ratio = fill_ratio;
                        min_fill_id = current_id;
                    }
                }

                if(min_fill_id < 0)
                {
                    throw Exception("min_fill_id < 0");
                }
                else
                {
                    sample->id = min_fill_id;
                    sample_ids.push_back(sample->id);
                }
#else
                // sort the class ids according to their (under)-fullness
                vector<Sortee> sortees(priority_group.classes.size());
                 
                for(unsigned int i = 0; i < priority_group.classes.size(); i++)
                {
                    const int current_id = priority_group.classes[i];
                    const float fill_ratio = num_samples_per_class[current_id]*1.0/target_num_samples_per_class[current_id];

                    sortees[i].id = current_id;
                    sortees[i].value = fill_ratio;
                }

                sort(sortees.begin(), sortees.end());

                for(unsigned int i = 0; i < sortees.size(); i++)
                {
                    sample_ids.push_back(sortees[i].id);
                }
#endif
            }
            else if(class_probability_all_int)
            {
                sample->id = static_cast<int>(floor(Random::UniformRandom()*priority_group.classes.size()))%priority_group.classes.size();
                sample->id = priority_group.classes[sample->id];
                sample_ids.push_back(sample->id);
            }
            else
            {
                if(priority_groups.size() != 1)
                {
                    throw Exception("priority_groups.size() != 1");
                }

                sample->id = RandomClass(class_probability);
                sample_ids.push_back(sample->id);
            }

            // while((target_num_samples_per_class[sample->id] > 0) && (num_samples_per_class[sample->id] >= target_num_samples_per_class[sample->id]));

#if 0   // useful debug
            cerr << "num_samples_per_class :";
            for(unsigned int i = 0; i < num_samples_per_class.size(); i++)
            {
                cerr << " " << num_samples_per_class[i];
            }
            cerr << " "; // << endl;
            cerr << "throw sample in class " << sample->id << endl;
#endif

            if(sample->coordinate.Dimension() != grid_domain_spec.domain_size.size())
            {
                throw Exception("sample->coordinate.size() != grid_domain_spec.domain_size.size()");
            }

            vector<float> sample_domain_min_corner(dimension);
            vector<float> sample_domain_max_corner(dimension);

#if defined(_USE_SINGLE_TREE) || defined(_USE_MULTI_TREE)
            if(sample_ids.size() == 1)
            {
                // do nothing
            }
            else
            {
                throw Exception("sample_ids.size() != 1");
            }
#endif

#if defined(_USE_MULTI_TREE)
            Tree & tree = multi_tree.GetTree(sample->id);
#endif

#if defined(_USE_SINGLE_TREE) || defined(_USE_MULTI_TREE)
#if defined(_USE_LEAF_NOT_POSSIBLE_CELLS)
            const int num_possible_cells = tree.NumLeafCells();
#else
            const int num_possible_cells = tree.NumPossibleCells(sample->id);
#endif

            // if(num_possible_cells <= 0)
            if(patience_factor*num_possible_cells < tree.GetVisitCount(sample_ids[0]))
            {
                // possible_cells.push_back(candidate_cells[0]);
                if(tree.Subdivide())
                {
                    // cerr << "tree subdivide into " << tree.NumLevels() << " levels" << endl;
                    continue;
                }
                else
                {
                    // cerr << "tree can no longer be subdivided" << endl;
                    subdividable[sample->id] = 0;
                    if(k_number <= 0)
                    {
                        break;
                    }
                    else
                    {
                        int non_subdividable = 1;
                        for(unsigned int i = 0; i < subdividable.size(); i++)
                        {
                            if(subdividable[i]) non_subdividable = 0;
                        }

                        if(non_subdividable) break;
                    }
                }
            }

            // pick a random cell
            const int selection = static_cast<int>(floor(Random::UniformRandom()*num_possible_cells))%num_possible_cells;

#if defined(_USE_LEAF_NOT_POSSIBLE_CELLS)
            Tree::Cell * sample_cell = tree.GetLeafCell(selection);
#else
            Tree::Cell * sample_cell = const_cast<Tree::Cell *>(tree.GetPossibleCell(sample_ids[0], selection));
#endif

#endif

#if defined(_USE_SINGLE_TREE) || defined(_USE_MULTI_TREE)
            // const int has_tried_hard_enough = (num_possible_cells > 0); // debug
            // cerr << "num_possible_cells: " << num_possible_cells << endl; // debug 
            // const int has_tried_hard_enough = (current_failures_per_class[sample->id] > patience_factor*target_num_samples_per_class[sample->id]); // same as grid
            const int has_tried_hard_enough = (current_failures_per_class[sample->id] > patience_factor*num_possible_cells); // tailor-made for tree class, tracking the number of possible cells
            // cerr << "has_tried_hard_enough " << has_tried_hard_enough << " = (" << current_failures_per_class[sample->id] <<  ">" << patience_factor << "*" << num_possible_cells << ")" << endl; // debug

#if 0 // (09/22/2008) decouple subdivision from sample removal
#ifndef _GRID_LIKE_TREE
            // no need to do the following if tree already pre-subdivided
            if(has_tried_hard_enough)
            {
                const int status = tree.Subdivide();

                if(status)
                {
                    // cerr << "tree subdivide into " << tree.NumLevels() << " levels" << endl;
                    continue; // skip the rest
                }
            }
#endif
#endif

#ifdef _PSYCHO_RHO_STRATEGY
            if((k_number == 0) && has_tried_hard_enough)
            {
                // pick any cell instead just possible ones  
                vector<Tree::Cell *> all_leaf_cells;
                if(!tree.GetCells(tree.NumLevels()-1, all_leaf_cells))
                {
                    throw Exception("cannot get leaf cells");
                }
                if(all_leaf_cells.size() <= 0)
                {
                    // cerr << "leaf level " << (tree.NumLevels()-1) << ", num cells " << tree.NumCells(tree.NumLevels()-1) << endl;
                    throw Exception("all_leaf_cells.size() <= 0");
                }
                const int selection = static_cast<int>(floor(Random::UniformRandom()*all_leaf_cells.size()))%all_leaf_cells.size();
                sample_cell = const_cast<Tree::Cell *>(all_leaf_cells[selection]);
                // cerr << "all_leaf_cells.size() " << all_leaf_cells.size() << endl;
            }
#endif
#else
            const int has_tried_hard_enough = (current_failures_per_class[sample->id] > patience_factor*target_num_samples_per_class[sample->id]);
            // const int has_tried_hard_enough = (current_failures_per_class[sample->id] > patience_factor*total_num_grid_cells);
#endif

#if defined(_USE_SINGLE_TREE) || defined(_USE_MULTI_TREE)
#if defined(_DISCRETE_DOMAIN)
#error cannot support tree and discrete domain together for now!
#endif
            if(! sample_cell) throw Exception("null sample_cell");

            sample_cell->GetDomain(sample_domain_min_corner, sample_domain_max_corner);
            sample_cell->AddVisitCount();
#else
            for(int i = 0; i < dimension; i++)
            {
                sample_domain_min_corner[i] = 0;
                sample_domain_max_corner[i] = grid_domain_spec.domain_size[i]*grid_domain_spec.cell_size;
            }
#endif

#if defined(_DISCRETE_DOMAIN)
            if(input_samples.size() <= 0) throw Exception("not more input samples to draw from");

            const int discrete_sample_selection = static_cast<int>(floor(Random::UniformRandom()*input_samples.size()))%input_samples.size();

            sample->coordinate = input_samples[discrete_sample_selection].coordinate;
   
#else
            for(int i = 0; i < sample->coordinate.Dimension(); i++)
            {
                sample->coordinate[i] = Random::UniformRandom()*(sample_domain_max_corner[i] - sample_domain_min_corner[i]) + sample_domain_min_corner[i];
            }
#endif

            // if(Random::UniformRandom() < 1.5) sample->coordinate[0] = 2*sample_domain_max_corner[0]; // debug, put the sample outside the domain

            if(sample_ids.size() <= 0)
            {
                throw Exception("sample_ids.size() <= 0");
            }

            SampleRecord::Status sample_fate = SampleRecord::UNKNOWN;

            for(unsigned int which_id = 0; (which_id < sample_ids.size()) && (sample_fate != SampleRecord::ACCEPTED); which_id++)
            {
                sample->id = sample_ids[which_id];

#if defined(_USE_SINGLE_TREE) || defined(_USE_MULTI_TREE)
                if(!sample_cell->Inside(*sample))
#else
                if(!grid.Inside(*sample))
#endif
                {
                    sample_fate = SampleRecord::OUTSIDE;
                }
#if defined(_USE_SINGLE_TREE)
                else if(!sample_cell->Conflict(*sample))
#elif defined(_USE_MULTI_TREE)
                else if(!multi_tree.Conflict(*sample))
#else
                else if(!grid.Conflict(*sample, conflict_checker))
#endif
                {
                    sample_fate = SampleRecord::ACCEPTED;
                }
                else // has conflict
                {
                    sample_fate = SampleRecord::REJECTED;

#if 0
                    if(current_failures_per_class[sample->id] > 0) // debug
                    {
                        cerr << "current_failures_per_class[" << sample->id << "]: " << current_failures_per_class[sample->id] << endl;
                    }
#endif

#ifdef _PSYCHO_RHO_STRATEGY
                    if((k_number == 0) && has_tried_hard_enough)
                    {
                        // cerr << "try killing" << endl; // debug
                        vector<const Sample *> neighbors;
#if defined(_USE_SINGLE_TREE)
                        if(!sample_cell->GetConflicts(*sample, neighbors))
#elif defined(_USE_MULTI_TREE)
                        if(!multi_tree.GetConflicts(*sample, neighbors))
#else
                        if(!grid.GetConflicts(*sample, conflict_checker, neighbors))
#endif
                        {
                            throw Exception("cannot get conflicts");
                        }

                        int neighbors_all_removable = 1;

                        const float sample_fill_ratio = num_samples_per_class[sample->id]*1.0/target_num_samples_per_class[sample->id];
                        float sample_r_value = 0;
                        r_matrix.Get(vector<int>(2, sample->id), sample_r_value);

                        for(unsigned int j = 0; j < neighbors.size(); j++)
                        {
                            const Sample & current_neighbor = *neighbors[j];

                        
                            float neighbor_r_value = 0;
                            r_matrix.Get(vector<int>(2, current_neighbor.id), neighbor_r_value);

                            if(neighbor_r_value >= sample_r_value)
                            {
                                neighbors_all_removable = 0;
                                break;
                            }

                            float current_neighbor_fill_ratio = 0;

                            for(unsigned int i = 0; i < priority_group.classes.size(); i++)
                            {
                                const int current_id = priority_group.classes[i];
                                if(current_neighbor.id == current_id)
                                {
                                    current_neighbor_fill_ratio = num_samples_per_class[current_id]*1.0/target_num_samples_per_class[current_id];
                                }
                            }

                            if((current_neighbor_fill_ratio <= 0) || (current_neighbor_fill_ratio < sample_fill_ratio))
                            {
                                neighbors_all_removable = 0;
                                break;
                            }
                        }

                        if(neighbors_all_removable)
                        {
                            for(unsigned int j = 0; j < neighbors.size(); j++)
                            {
#if defined(_USE_SINGLE_TREE)
                                if(! tree.Remove(*neighbors[j]))
#elif defined(_USE_MULTI_TREE)
                                if(! multi_tree.Remove(*neighbors[j]))
#else
                                if(! grid.Remove(*neighbors[j]))
#endif
                                {
                                    throw Exception("cannot remove neighbor");
                                }
#if defined(_USE_SINGLE_TREE)
                                if(! tree.RemoveDeadZone(*neighbors[j]))
#elif defined(_USE_MULTI_TREE)
                                if(! multi_tree.RemoveDeadZone(*neighbors[j]))
                                {
                                    throw Exception("cannot remove dead zone");
                                }
#endif
                                num_samples--;
                                total_num_samples--;
                                num_samples_per_class[neighbors[j]->id] -= 1;

#ifdef _RECORD_SAMPLE_HISTORY
                                const SampleRecord new_record(neighbors[j]->id, SampleRecord::KILLED);
                                sample_history.push_back(new_record);
#endif

#if defined(_DISCRETE_DOMAIN)
                                input_samples.push_back(*neighbors[j]);
#endif
                                delete neighbors[j];
                            }

                            neighbors.clear();
#if defined(_USE_SINGLE_TREE)
                            if(!sample_cell->Conflict(*sample))
#elif defined(_USE_MULTI_TREE) 
                            if(!multi_tree.Conflict(*sample))
#else
                            if(!grid.Conflict(*sample, conflict_checker))
#endif
                            {
                                sample_fate = SampleRecord::ACCEPTED;
                            }
                            else
                            {
                                throw Exception("still conflict after removing neighbors");
                            }
                        }
                        else
                        {
                            // cerr << "cannot remove neighbors" << endl; // debug
                        }
                    }
#endif
                }

#if defined(_DISCRETE_DOMAIN)
                if(sample_fate == SampleRecord::ACCEPTED)
                {  
                    input_samples[discrete_sample_selection] = input_samples[input_samples.size()-1];
                    input_samples.pop_back();
                }
#endif

                if(sample_fate == SampleRecord::ACCEPTED)
                {
                    current_failures_per_class[sample->id] = 0;
                }
                else
                {
                    current_failures_per_class[sample->id] += 1;
                }

                switch(sample_fate)
                {
                case SampleRecord::ACCEPTED:
                {
#ifdef _RECORD_SAMPLE_HISTORY
                    const SampleRecord new_record(sample->id, SampleRecord::ACCEPTED);
                    sample_history.push_back(new_record);
#endif
#if defined(_USE_SINGLE_TREE)
                    if(!sample_cell->Add(*sample))
#elif defined(_USE_MULTI_TREE)
                    if(!multi_tree.Add(*sample))
#else
                    if(!grid.Add(*sample))
#endif
                    {
                        throw Exception("cannot add sample");
                        delete sample; sample = 0;
                    }
                    else
                    {
#if defined(_USE_SINGLE_TREE) || defined(_USE_MULTI_TREE)
#if defined(_USE_SINGLE_TREE)
                        if(!sample_cell->AddDeadZone(*sample))
#else
                        if(!multi_tree.AddDeadZone(*sample))
#endif
                        {
                            throw Exception("cannot add deadzone");
                            delete sample; sample = 0;
                        }
#endif
                        num_samples++;
                        total_num_samples++;
                        num_samples_per_class[sample->id] += 1;
                        sample = 0;
                    }
                    break;
                }

                case SampleRecord::REJECTED:
                {
#ifdef _RECORD_SAMPLE_HISTORY
                    const SampleRecord new_record(sample->id, SampleRecord::REJECTED);
                    sample_history.push_back(new_record);
#endif
                    break;
                }

                case SampleRecord::OUTSIDE:
                    // do nothing
                    break;

                default:
                    throw Exception("unknown sample fate");
                    break;
                }
            }
            
            if(sample != 0)
            {
                delete sample; sample = 0;
            }
        }
    }

    timer.Stop();
    const double total_time = timer.ElapsedTime();
    cerr << "total time " << total_time << endl;

    // output
    vector<const Sample *> samples;
#if defined(_USE_SINGLE_TREE)
    tree.GetSamples(samples);

#if 1 // debug
    for(int level = 0; level < tree.NumLevels(); level++)
    {
        vector<int> num_cells(dimension);

        tree.GetNumCellsPerDimension(level, num_cells);

        cerr << "tree level " << level << " ";
        cerr << "theory_num_cells:";
        for(unsigned int i = 0; i < num_cells.size(); i++)
        {
            cerr << " " << num_cells[i];
        }
        cerr << endl;
    }
#endif
#elif defined(_USE_MULTI_TREE)
    multi_tree.GetSamples(samples);

#if 0 // debug
    for(int which_tree = 0; which_tree < multi_tree.NumTrees(); which_tree++)
    {
        Tree & tree = multi_tree.GetTree(which_tree);

        for(int level = 0; level < tree.NumLevels(); level++)
        {
            vector<int> num_cells(dimension);

            tree.GetNumCellsPerDimension(level, num_cells);

            cerr << "tree " << which_tree << " level " << level << " ";
            cerr << "theory_num_cells:";
            for(unsigned int i = 0; i < num_cells.size(); i++)
            {
                cerr << " " << num_cells[i];
            }
            cerr << endl;
        }
    }
#endif
#else
    grid.GetSamples(samples);
#endif

    if(total_num_samples != samples.size())
    {
        throw Exception("total_num_samples != samples.size()");
    }

    cerr << samples.size() << " samples" << endl;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        cout << samples[i]->id;
        for(int j = 0; j < samples[i]->coordinate.Dimension(); j++)
        {
            cout << " " << samples[i]->coordinate[j];
        }
        cout << endl;
    }

#if defined(_DISCRETE_DOMAIN)
    for(unsigned int i = 0; i < input_samples.size(); i++)
    {
        cout << input_samples[i].id;
        for(int j = 0; j < input_samples[i].coordinate.Dimension(); j++)
        {
            cout << " " << input_samples[i].coordinate[j];
        }
        cout << endl;
    }
#endif

#ifdef _RECORD_SAMPLE_HISTORY
    if(history_file_name)
    {
        if(! WriteSampleHistory(history_file_name, sample_history))
        {
            cerr << "cannot write to " << history_file_name << endl;
        }
    }
    else
    {
        cerr << "null history file name" << endl;
    }
#endif

    cerr << "# samples per second " << samples.size()/total_time << endl;

    // clean up
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        delete samples[i];
    }
    samples.clear();

#if defined(_ADAPTIVE_SAMPLING)
    if(sampled_distance_field)
    {
        delete sampled_distance_field; sampled_distance_field = 0;
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
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
