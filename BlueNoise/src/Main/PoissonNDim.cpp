/*
  Poission disk distribution in arbitrary dimensions

  The first algorithm is based on
  Fast poisson disk sampling in arbitrary dimensions
  Robert Bridson, ACM SIGGRAPH 2007 Sketches Program

  The other algorithms are my innovation

  Right now, the best algorithm is multi-resolution + random order (07/01/2007)
  
  Li-Yi Wei
  06/28/2007 (project initiated)
  07/01/2007 (multi-resolution + random order is working well)

  Todo
 */

#include <iostream>
#include <vector>
#include <deque>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "Random.hpp"
#include "Grid.hpp"
#include "SequentialCounter.hpp"
#include "PhaseRandomCounter.hpp"

struct MultiResCellIndex
{
public:
    MultiResCellIndex(const float cell_size_in, const int dim) : cell_size(cell_size_in), coordinate(dim)
        {
        }
public:
    float cell_size;
    CoordinateT<int> coordinate;
};

struct LevelSampleSpec
{
public:
    LevelSampleSpec(const float cell_size_in, const int num_cells_per_dim_in): cell_size(cell_size_in), num_cells_per_dim(num_cells_per_dim_in)
        {
        }
public:
    float cell_size;
    int num_cells_per_dim;
};

struct KCounter
{
public:
    KCounter(void): cell_size(0), num_samples(0), accumulated_k(0)
        {
        }
    
public:
    float cell_size; // resolution information
    unsigned long num_samples;
    unsigned long accumulated_k;
};

int main(int argc, char *argv[])
{
    if(argc < 8)
    {
        cerr << "Usage: " << argv[0] << " dimension r k (r and k are as in Bridson's SIGGRAPH 2007 sketch; k is usually 30 according to Bridson) order (0 for bridson, 1 for scanline, 2 for random, 3 for GPU-random-phase, 4 for random dart, 5 for gpu-scanline-phase) num_resolutions (<= 0 for maximum) clique_size (<= 0 for natural one) max_num_samples (< 0 for no limit)" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float radius = atof(argv[++argCtr]);
    const int k_number = atoi(argv[++argCtr]);
    const int synthesis_order = atoi(argv[++argCtr]);
    const int num_resolutions = atoi(argv[++argCtr]);
    const int clique_size_option = atoi(argv[++argCtr]);
    const int max_num_samples = atoi(argv[++argCtr]);
    
    const int clique_size = ((synthesis_order == 3 || synthesis_order == 5) ? (clique_size_option > 0 ? clique_size_option : Grid::CliqueSize(dimension)) : 1); // for GPU synthesis order;
    
    if(dimension <= 1)
    {
        cerr << "dimension must be >= 2" << endl;
        return 1;
    }

    if((radius <= 0) || (radius >= 1))
    {
        cerr << "r must be within (0 1)" << endl;
        return 1;
    }

    if((k_number <= 0))
    {
        cerr << "k must be > 0 (typically 30 according to Robert Bridson)" << endl;
        return 1;
    }         

    if((synthesis_order < 0) || (synthesis_order > 5))
    {
        cerr << "unknown synthesis_order" << endl;
        return 1;
    }

    // grid
    Grid grid(dimension, radius, clique_size);

    if(grid.NumCellsPerDimension()%clique_size)
    {
        cerr << "number of cells per dimension is not an integer multiple of clique size" << endl;
        return 1;
    }

    cerr << "clique size: " << clique_size << endl;

#if 0
    cerr << "num cells per dimension: " << grid.NumCellsPerDimension() << ", cell_size: " << grid.CellSize() << endl;
    
    const int cell_offset = static_cast<int>(ceil(radius/grid.CellSize()));
    cerr << "cell_offset " << cell_offset << endl;
#endif
    
    // init
    Random::InitRandomNumberGenerator();

    if(synthesis_order == 0)
    {
        // seed sample
        cerr << "bridson synthesis order" << endl;
        
        cerr << "produce initial sample" << endl;
    
        deque<const Sample *> active_list;
    
        Sample * sample = new Sample(dimension);
        {
            for(int i = 0; i < sample->coordinate.Dimension(); i++)
            {
                sample->coordinate[i] = Random::UniformRandom();
            }
        }

        if(!grid.Inside(*sample))
        {
            cerr << "strange initial outside" << endl;

            {
                for(int i = 0; i < sample->coordinate.Dimension(); i++)
                {
                    cerr << sample->coordinate[i] << " ";
                }
                cerr << endl;
            }
        
            return 1;
        }

        if(grid.Conflict(*sample, radius))
        {
            cerr << "strange initial conflict" << endl;
            return 1;
        }

        if(!grid.Add(*sample))
        {
            cerr << "cannot add sample to the grid" << endl;
            return 1;
        }

        active_list.push_back(sample);
    
        // more samples
        cerr << "produce more samples" << endl;
    
        while(active_list.size() > 0)
        {
            //cout << active_list.size() << " ";
        
            const int anchor_index = static_cast<int>(floor(Random::UniformRandom()*active_list.size()));

            const Sample & anchor = *active_list[anchor_index];

            // produce k random samples
            int any_k_sample_added = 0;
            // right now, this process will likely produce corner bias
            // need to improve in the future
            for(int i = 0; i < k_number; i++)
            {
                sample = new Sample(dimension);
            
                const float random_radius = (Random::UniformRandom()+1)*radius;

                float magnitude = 0;
                do
                {
                    magnitude = 0;
                    for(int j = 0; j < sample->coordinate.Dimension(); j++)
                    {
                        sample->coordinate[j] = Random::UniformRandom()-0.5;
                        magnitude += sample->coordinate[j]*sample->coordinate[j];
                    }

                    magnitude = sqrt(magnitude);
                }
                while(magnitude > 0.5); // to avoid bias towards corners
            
                {
                    for(int j = 0; j < sample->coordinate.Dimension(); j++)
                    {
                        sample->coordinate[j] = sample->coordinate[j]*random_radius/magnitude + anchor.coordinate[j];
                    }       
                }

                // try to see if there is conflict
                if(grid.Inside(*sample) && !grid.Conflict(*sample, radius))
                {
                    if(! grid.Add(*sample))
                    {
                        cerr << sample->coordinate[1] << " " << sample->coordinate[0] << endl;
                        return 1;
                    }

                    active_list.push_back(sample);
                
                    any_k_sample_added = 1;
                }
            }

            if(! any_k_sample_added)
            {
                // remove anchor
                if(active_list.size() == 0)
                {
                    active_list.pop_back();
                }
                else if(anchor_index > 0)
                {
                    active_list[anchor_index] = active_list[0];
                    active_list.pop_front();
                }
                else
                {
                    active_list[anchor_index] = active_list[active_list.size()-1];
                    active_list.pop_back();
                }
            }
        }
    }
    else
    {
        cerr << "figure out the synthesis order " << endl;

        const int num_cells_per_dim = grid.NumCellsPerDimension();
        int max_num_resolutions = 0;
        
        deque<LevelSampleSpec> pyramid_sample_spec;
        for(int i = 1; i < num_cells_per_dim; i*=2)
        {
            LevelSampleSpec level_sample_spec(pow(2.0, -max_num_resolutions), i);
            pyramid_sample_spec.push_front(level_sample_spec);
            max_num_resolutions++;
        }
        {
            LevelSampleSpec level_sample_spec(grid.CellSize(), num_cells_per_dim);
            pyramid_sample_spec.push_front(level_sample_spec);
            max_num_resolutions++;
        }
        
        if((num_resolutions > 0) && (max_num_resolutions > num_resolutions))
        {
            max_num_resolutions = num_resolutions;
        }
        
        deque< MultiResCellIndex > multires_cell_index_list;
        
        for(int level = max_num_resolutions-1; level >= 0; level--)
        {
            assert(level < pyramid_sample_spec.size());
            
            deque< MultiResCellIndex > cell_index_list;

            cerr << "level " << level << ", cell_size " << pyramid_sample_spec[level].cell_size << ", num_cells_per_dim " << pyramid_sample_spec[level].num_cells_per_dim << endl;
            
            // scanline
            if(synthesis_order != 3) // no need for gpu phase random
            {
                MultiResCellIndex current_cell_index(pyramid_sample_spec[level].cell_size, dimension);
                
                SequentialCounter cell_index_counter(dimension, 0, pyramid_sample_spec[level].num_cells_per_dim-1);

                cell_index_counter.Reset();
                
                do
                {
                    vector<int> current_coordinate;
                    cell_index_counter.Get(current_coordinate);

                    assert(current_coordinate.size() == current_cell_index.coordinate.Dimension());
                    
                    {
                        for(unsigned int i = 0; i < current_coordinate.size(); i++)
                        {
                            current_cell_index.coordinate[i] = current_coordinate[i];
                        }
                    }
                    
                    cell_index_list.push_back(current_cell_index);
                }
                while(cell_index_counter.Next());
                // cerr << "cell_index_list.size() = " << cell_index_list.size() << endl;
            }
            
            if(synthesis_order == 1)
            {
                // scanline
                // do nothing

                cerr << "scanline synthesis order" << endl;
            }
            else if(synthesis_order == 2)
            {
                // random

                cerr << "random order" << endl;
                
                deque< MultiResCellIndex > random_cell_index_list;

                while(cell_index_list.size() > 0)
                {
                    const int selection = static_cast<int>(floor(Random::UniformRandom()*(cell_index_list.size()-1)));

                    random_cell_index_list.push_back(cell_index_list[selection]);

                    cell_index_list[selection] = cell_index_list[0];
                    cell_index_list.pop_front();
                }

                cell_index_list = random_cell_index_list;
            }
            else if(synthesis_order == 3)
            {
                cerr << "gpu random synthesis order with clique size (per dim) " << clique_size << endl;

                cell_index_list.clear(); // no need for the stuff pre-built for scanline order
                
                MultiResCellIndex current_cell_index(pyramid_sample_spec[level].cell_size, dimension);
                
                PhaseRandomCounter cell_index_counter(dimension, 0, pyramid_sample_spec[level].num_cells_per_dim-1, clique_size);

                //cerr << "foo: " << level << endl;
                cell_index_counter.Reset();
                //cerr << "bar: " << level << endl;
                
                do
                {
                    vector<int> current_coordinate;
                    cell_index_counter.Get(current_coordinate);

                    assert(current_coordinate.size() == current_cell_index.coordinate.Dimension());
                    
                    {
                        for(unsigned int i = 0; i < current_coordinate.size(); i++)
                        {
                            current_cell_index.coordinate[i] = current_coordinate[i];
                        }
                    }
                    
                    cell_index_list.push_back(current_cell_index);
                }
                while(cell_index_counter.Next());
            }
            else if(synthesis_order == 5)
            {
                cerr << "gpu scanline synthesis order with clique size (per dim) " << clique_size << endl;
                
                deque< MultiResCellIndex > clique_cell_index_list;

                MultiResCellIndex current_phase_index(-1, dimension); // the first parameter is not used at all

                SequentialCounter phase_counter(dimension, 0, clique_size-1);
                phase_counter.Reset();

		do
                {
                    vector<int> current_coordinate;
                    phase_counter.Get(current_coordinate);

                    assert(current_coordinate.size() == current_phase_index.coordinate.Dimension());
                    
                    {
                        for(unsigned int i = 0; i < current_coordinate.size(); i++)
                        {
                            current_phase_index.coordinate[i] = current_coordinate[i];
                        }
                    }
                    
                    // put proper clique phase from cell_index_list into clique_cell_index_list
                    for(unsigned int i = 0; i < cell_index_list.size(); i++)
                    {
                        int all_dim_good = 1;

                        for(int j = 0; j < dimension; j++)
                        {
                            if((cell_index_list[i].coordinate[j]%clique_size) != current_phase_index.coordinate[j])
                            {
                                all_dim_good = 0;
                            }
                        }

                        if(all_dim_good)
                        {
                            clique_cell_index_list.push_back(cell_index_list[i]);
                        }
                    }
                }
                while(phase_counter.Next());

                assert(cell_index_list.size() == clique_cell_index_list.size());
                
                cell_index_list = clique_cell_index_list;
            }
            else if(synthesis_order == 4)
            {
                // dart throwing
                // do nothing

                cerr << "dart throwing" << endl;
            }
            else
            {
                // unknown
                cerr << "unknown synthesis order" << endl;
                return 1;
            }

            // add to total list
            while(cell_index_list.size() > 0)
            {
                MultiResCellIndex entry = cell_index_list[0];
                cell_index_list.pop_front();
                multires_cell_index_list.push_back(entry);
            }
        }
        
        // add samples
        cerr << "add samples" << endl;

        deque< KCounter > k_counters;

        unsigned long total_num_samples = 0;
        int enough_samples = 0;
        // const float cell_size = grid.CellSize();

        while((!enough_samples) && (multires_cell_index_list.size() > 0))
        {
            const MultiResCellIndex current_cell_index = multires_cell_index_list[0];
            multires_cell_index_list.pop_front();

            Sample * sample = new Sample(dimension);

            for(int trial = 0; (trial < k_number) && (sample) && (!enough_samples); trial++)
            {
                for(int i = 0; i < dimension; i++)
                {
                    sample->coordinate[i] = synthesis_order == 4 ? Random::UniformRandom() : (current_cell_index.coordinate[i]+Random::UniformRandom())*current_cell_index.cell_size;
                }
                
                // try to see if there is conflict
                if(grid.Inside(*sample) && !grid.Conflict(*sample, radius))
                {
                    if(! grid.Add(*sample))
                    {
                        cerr << "cannot add sample (" << sample->coordinate[1] << " " << sample->coordinate[0] << ")" << endl;
                        return 1;
                    }

                    total_num_samples++;
                    sample = 0;

                    enough_samples = ((max_num_samples >= 0) && (total_num_samples >= max_num_samples));
                    
                    // sample successfully added, quit (if not dart throwing)
                    if(synthesis_order != 4)
                    {
                        break;
                    }
                }
            }

            if(sample)
            {
                delete sample;
                sample = 0;
            }
        }
    }
    
    // output samples
    cerr << "output samples" << endl;
    
    vector<const Sample *> samples;
    grid.GetSamples(samples);

    {
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            assert(samples[i]);
            
            for(int j = 0; j < samples[i]->coordinate.Dimension(); j++)
            {
                cout << samples[i]->coordinate[j] << " ";
            }
            cout << endl;
        }
    }
        
    // release samples
    cerr << "release samples" << endl;
    {
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            delete samples[i];
        }
    }
    
    // done
    cerr << "done" << endl;
    return 0;
}
