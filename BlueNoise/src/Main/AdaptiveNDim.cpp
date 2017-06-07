/*
  AdaptiveNDim.cpp

  parallel adaptive Poisson disk sampling

  Li-Yi Wei
  07/15/2007 : try the uniform distance field version first

*/

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>

#include "Exception.hpp"
#include "Random.hpp"
#include "KDTree.hpp"
#include "UniformDistanceField.hpp"
#include "RampDistanceField.hpp"
#include "SampledDistanceField.hpp"
#include "Math.hpp"
#include "FrameBuffer.hpp"
#include "PhaseGroup.hpp"

// #define _RECORD_K_STATISTICS
// #define _RECORD_CONFLICT_CHECK_STATISTICS

#ifdef _RECORD_CONFLICT_CHECK_STATISTICS
struct ConflictMarker
{
    ConflictMarker(void) : num_conflict_checks(0), num_samples(0) {};
    
    double num_conflict_checks;
    double num_samples;
};
#endif

int ReadDistanceField(const int dimension, const float min_distance, const char * distance_field_file_name, Array<float> & distance_field_data)
{
    const int max_num_samples = Math::ComputeMaxNumSamples(dimension, min_distance);
    
    // read in image, recording importance
    Array<FrameBuffer::PF> image;

    if(! FrameBuffer::ReadPFM(distance_field_file_name, image))
    {
        return 0;
    }

    if(image.Dimension() != dimension) return 0;
    
    // convert into gray scale importance
    vector<int> field_size;
    image.Size(field_size);

    distance_field_data = Array<float>(field_size);

    vector<FrameBuffer::PF> input_data;
    vector<float> output_data;

    if(! image.Get(input_data)) return 0;
    if(! distance_field_data.Get(output_data)) return 0;

    assert(input_data.size() == output_data.size());

    for(unsigned int i = 0; i < output_data.size(); i++)
    {
        output_data[i] = input_data[i].g;
    }
    
    if(! distance_field_data.Put(output_data)) return 0;

    // done
    return 1;
}

int ComputePhaseGroups(const PhaseGroup::Order order, KDTree & tree, const int level, vector< vector<KDTree::Cell *> > & phase_groups)
{
    return PhaseGroup::Compute(order, tree, level, phase_groups);
}

int Main(int argc, char **argv)
{
    // process input arguments
    const int num_fixed_arguments = 7;
    if(argc < num_fixed_arguments)
    {
        cerr << "Usage: " << "dimension r k synthesis_order (sequential_random parallel_onion parallel_random_template) cell_size importance_image_name importance_field_size (dimension numbers)" << endl;
        return 1;
    }
    
    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float min_distance = atof(argv[++argCtr]);
    const int outer_k_number = atoi(argv[++argCtr]);
    const int inner_k_number = 1; // atoi(argv[++argCtr]);
    const PhaseGroup::Order order = static_cast<PhaseGroup::Order>(atoi(argv[++argCtr]));
    const float cell_size = atof(argv[++argCtr]);
    const char * importance_file_name = argv[++argCtr];
    if(argc < num_fixed_arguments+dimension)
    {
        cerr << "insufficient number of importance_size arguments" << endl;
        return 1;
    }

    // init stuff
    vector<float> range_min(dimension);
    vector<float> range_max(dimension);
    {
        for(int i = 0; i < dimension; i++)
        {
            range_min[i] = 0.0;
            range_max[i] = atof(argv[++argCtr]); //instead of 1.0
        }
    }

    // random seed
    const unsigned long random_seed = ((argCtr+1) < argc ? atoi(argv[++argCtr]) : time(0));

    DistanceField * p_distance_field = 0;
    if(!strstr(importance_file_name, ".pfm"))
    {
        if(strstr(importance_file_name, "uniform"))
        {
            p_distance_field = new UniformDistanceField(range_min, range_max, min_distance);
        }
        else if(strstr(importance_file_name, "ramp"))
        {
            p_distance_field = new RampDistanceField(range_min, range_max, min_distance);
        }
        else
        {
            cerr << "unknown built-in distance field" << endl;
            return 1;
        }
    }
    else
    {
        Array<float> importance;
        if(! ReadDistanceField(dimension, min_distance, importance_file_name, importance))
        {
            cerr << "error in reading " << importance_file_name << endl;
            return 1;
        }
        
        p_distance_field = new SampledDistanceField(range_min, range_max, min_distance, importance);
    }

    if(p_distance_field == 0)
    {
        cerr << "cannot allocate distance field" << endl;
        return 1;
    }
    
    DistanceField & distance_field = *p_distance_field;
    
    KDTree tree(dimension, cell_size, distance_field);

    Random::InitRandomNumberGenerator(random_seed);

#ifdef _RECORD_K_STATISTICS
    vector< vector<int> > num_samples_level_k;
#endif

#ifdef _RECORD_CONFLICT_CHECK_STATISTICS
    vector<ConflictMarker> conflict_markers;
#endif
 
    // adaptive sampling from low to high resolutions
    do
    {
        const int current_level = tree.NumLevels()-1;

#ifdef _RECORD_K_STATISTICS
        if(num_samples_level_k.size() != (tree.NumLevels()-1))
        {
            throw Exception("num_samples_level_k.size() != (tree.NumLevels()-1)");
        }

        num_samples_level_k.push_back(vector<int>(outer_k_number));
        for(int i = 0; i < num_samples_level_k[current_level].size(); i++)
        {
            num_samples_level_k[current_level][i] = 0;
        }
#endif
        
        int num_neighbors_checked = 0;
#ifdef _RECORD_CONFLICT_CHECK_STATISTICS
        if(conflict_markers.size() != (tree.NumLevels()-1))
        {
            throw Exception("conflict_markers.size() != (tree.NumLevels()-1)");
        }

        conflict_markers.push_back(ConflictMarker());

        conflict_markers[current_level].num_conflict_checks = 0;
        conflict_markers[current_level].num_samples = 0;
#endif
        
        vector<int> num_cells_per_dim(dimension);

        tree.GetNumCellsPerDimension(current_level, num_cells_per_dim);

        {
            cerr << "level: " << current_level << ", grid size: ";

            for(int i = 0; i < dimension; i++)
            {
                cerr << num_cells_per_dim[i] << " ";
            }
            cerr << ", # cells: " << tree.NumCells(current_level) << endl;
        }

        for(int outer_trial = 0; outer_trial < outer_k_number; outer_trial++)
        {
            vector< vector<KDTree::Cell *> > phase_groups;
            if(! ComputePhaseGroups(order, tree, current_level, phase_groups))
            {
                cerr << "cannot compute phase groups" << endl;
                return 1;
            }

            if(outer_trial == 0)
            {
                cerr << "phase group # " << phase_groups.size() << endl;
            }
            
            const float cell_size = tree.CellSize(current_level);
            vector<int> cell_index(dimension);
        
            for(unsigned int i = 0; i < phase_groups.size(); i++)
                for(unsigned int j = 0; j < phase_groups[i].size(); j++)
                {
                    KDTree::Cell * current_cell = phase_groups[i][j];

                    if(current_cell)
                    {
                        if(! current_cell->GetIndex(cell_index))
                        {
                            cerr << "error in getting cell index" << endl;
                            return 1;
                        }

                        assert(cell_index.size() == dimension);
                    
                        for(int inner_trial = 0; (inner_trial < inner_k_number) && (current_cell->NumSamples() == 0); inner_trial++)
                        {
                            Sample * sample = new Sample(dimension);
                            assert(sample);
                        
                            for(int n = 0; n < dimension; n++)
                            {
                                sample->coordinate[n] = (cell_index[n]+Random::UniformRandom())*cell_size;
                            }

                            if((tree.GetDistanceField().Query(sample->coordinate) < FLT_MAX) && !current_cell->EccentricConflict(*sample, num_neighbors_checked) && distance_field.Inside(sample->coordinate))
                            {
                                if(! current_cell->Add(*sample))
                                {
                                    cerr << "cannot add sample" << endl;
                                    return 1;
                                }
                            }
                            else
                            {
                                delete sample; sample = 0;
                            }
#ifdef _RECORD_CONFLICT_CHECK_STATISTICS
                            conflict_markers[current_level].num_conflict_checks += num_neighbors_checked;
                            conflict_markers[current_level].num_samples += 1;
#endif
                        }
                    }
                }
#ifdef _RECORD_K_STATISTICS
            {
                vector<const Sample *> samples;
            
                tree.GetSamples(samples);
                num_samples_level_k[current_level][outer_trial] = samples.size();
            }
#endif
        }

        // debugging info
        if(0)
        {
            vector<const Sample *> samples;
            
            tree.GetSamples(samples);

            cerr << samples.size() << " samples at level " << current_level << endl;
        }
    }   
    while(tree.Subdivide());

    // output stuff
    cerr << "output stuff " << endl;
    vector<const Sample *> samples;

    tree.GetSamples(samples);

    {
        // cerr << "expected # of samples " << Math::ComputeMaxNumSamples(dimension, min_distance) << endl; // meaningless message
        cerr << "total # of samples " << samples.size() << endl;
                
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            if(samples[i])
            {
                for(unsigned int j = 0; j < samples[i]->coordinate.Dimension(); j++)
                {
                    cout << samples[i]->coordinate[j] << " ";
                }
                cout << endl;
            }
        }
    }
    
#ifdef _RECORD_K_STATISTICS
    {
        cerr << "output k statistics" << endl;
        for(int i = 0; i < num_samples_level_k.size(); i++)
        {
            cerr << "level " << i << " : ";
            for(int j = 0; j < num_samples_level_k[i].size(); j++)
            {
                cerr << (num_samples_level_k[i][j]*1.0/num_samples_level_k[i][num_samples_level_k[i].size()-1]) << " ";
            }
            cerr << endl;
        }
    }
#endif

#ifdef _RECORD_CONFLICT_CHECK_STATISTICS
    {
        cerr << "# conflict checks : ";
        double total_num_conflict_checks = 0;
        double total_num_samples = 0;
        for(unsigned int i = 0; i < conflict_markers.size(); i++)
        {
            total_num_conflict_checks += conflict_markers[i].num_conflict_checks;
            total_num_samples += conflict_markers[i].num_samples;
            cerr << conflict_markers[i].num_conflict_checks << "/" << conflict_markers[i].num_samples << " ";
        }
        cerr << " - average : " << total_num_conflict_checks << "/" << total_num_samples << " == " << total_num_conflict_checks/total_num_samples << endl;
    }
#endif
    
    // done
    cerr << "done" << endl;
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
        cerr << e.Message() << endl;
        return 1;
    }
}
