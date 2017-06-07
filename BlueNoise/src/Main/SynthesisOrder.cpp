/*
  SynthesisOrder.cpp

  dump out sequences of random numbers produced from various synthesis orders
  useful for further statistical analysis e.g. DIEHARD
  
  Li-Yi Wei
  11/06/2007

*/

#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>

// #define _SORTED_RANDOM

#include "Array.hpp"
#include "SequentialCounter.hpp"
#include "RandomCounter.hpp"
#include "PhaseRandomCounter.hpp"
#include "Random.hpp"
#include "MDHash.hpp"
#include "Exception.hpp"
#include "Grid.hpp"

unsigned long Flip(const unsigned long input) // seems to only make things worse
{
    unsigned long output = 0;

    for(int i = 0; i < 32; i++)
    {
        output = (output << 1) | ((input >> i)%2);
    }

    return output;
}

int Pack(const vector<int> & index, unsigned long & packed_index, const int num_bits)
{
    if(num_bits*index.size() > sizeof(packed_index)*8)
    {
        return 0;
    }

    packed_index = (packed_index >> (num_bits*index.size()));
    
    const unsigned long mask = ((1 << num_bits) -1);
    
    for(unsigned int i = 0; i < index.size(); i++)
    {
        const unsigned long value = index[i] & mask;
        packed_index = (packed_index << num_bits) | value;
    }
    
    return 1;
}

#ifdef _SORTED_RANDOM
unsigned long Query(const unsigned long index, const vector<unsigned long> & data)
{
    if(index < data.size())
    {
        return data[index];
    }
    else
    {
        throw Exception("out of bound input for Query()");
        return 0;
    }
}
#endif

typedef enum{HASH, RANDOM, SCANLINE, GRID_RANDOM, GRID_SCANLINE} SynthesisOrder;

int Main(int argc, char **argv)
{
    const unsigned long time_stamp = time(0);
    Random::InitRandomNumberGenerator(time_stamp);
    
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " dimension size_per_dim synthesis_order (0 for hash, 1 for random, 2 for scanline, 3 for grid_partition_random_phase, 4 for grid_partition_scanline_phase) output_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int size_per_dim = atoi(argv[++argCtr]);
    const SynthesisOrder synthesis_order = static_cast<SynthesisOrder>(atoi(argv[++argCtr]));
    const char *output_file_name = argv[++argCtr];

    // compute number of bits
    const int num_bits = static_cast<int>(floor(log(size_per_dim*1.0)/log(2.0) + 0.5));

    if((1 << num_bits) != size_per_dim)
    {
        cerr << "size_per_dim " << size_per_dim << " not power of 2" << endl;
        return 1;
    }

    if(num_bits*dimension > sizeof(unsigned long)*8)
    {
        cerr << "too many bits: " << num_bits << "*" << dimension << " > " << sizeof(unsigned long)*8 << endl;
        return 1;
    }

    MDHash hash;
    
#ifdef _SORTED_RANDOM
    // build sorted random
    vector<unsigned long> sorted_randoms(pow(size_per_dim, dimension));

    for(unsigned long i = 0; i < sorted_randoms.size(); i++)
    {
        MDHash::MessageBlock input_message(i^time_stamp);
        MDHash::MessageBlock output_message;
        
        hash.Hash(input_message, output_message);
            
        sorted_randoms[i] = output_message;
    }

    sort(sorted_randoms.begin(), sorted_randoms.end());
#endif

    // build order
    const vector<int> data_size_input(dimension, size_per_dim);
    Array<unsigned long> data(data_size_input);

    if(synthesis_order == HASH) // mainly for debug
    {
        cerr << "synthesis_order == HASH" << endl;
        
        SequentialCounter counter(dimension, 0, size_per_dim-1);
        counter.Reset();

        vector<int> index;
        do
        {
            counter.Get(index);

            unsigned long packed_index = 0;

            if(!Pack(index, packed_index, num_bits))
            {
                cerr << "error in packing" << endl;
                return 1;
            }
            
            MDHash::MessageBlock input_message(packed_index);
            MDHash::MessageBlock output_message;

            hash.Hash(input_message, output_message);
            
            const unsigned long output = output_message;

            data.Put(index, output);
        }
        while(counter.Next());
    }
    else if(synthesis_order == RANDOM)
    {  
        cerr << "synthesis_order == RANDOM" << endl;
        
        RandomCounter counter(dimension, 0, size_per_dim-1);
        SequentialCounter sequential_counter(dimension, 0, size_per_dim-1);
        
        counter.Reset();
        sequential_counter.Reset();
        
        vector<int> index;
        do
        {
            counter.Get(index);

            unsigned long packed_index = 0;

            if(!Pack(index, packed_index, num_bits))
            {
                cerr << "error in packing" << endl;
                return 1;
            }
            
#ifdef _SORTED_RANDOM
            const unsigned long output = Query(packed_index, sorted_randoms);
#else
            MDHash::MessageBlock input_message(packed_index);
            MDHash::MessageBlock output_message;

            hash.Hash(input_message, output_message);
            
            unsigned long output = output_message;

            if(!Pack(index, output, num_bits))
            {
                cerr << "error in packing" << endl;
                return 1;
            }
#endif
            sequential_counter.Get(index);
            data.Put(index, output);
        }
        while(counter.Next() && sequential_counter.Next());
    }
    else if(synthesis_order == SCANLINE)
    {  
        cerr << "synthesis_order == SCANLINE" << endl;
        
        SequentialCounter counter(dimension, 0, size_per_dim-1);
        counter.Reset();

        vector<int> index;
        do
        {
            counter.Get(index);

            unsigned long packed_index = 0;

            if(!Pack(index, packed_index, num_bits))
            {
                cerr << "error in packing" << endl;
                return 1;
            }
            
#ifdef _SORTED_RANDOM
            const unsigned long output = Query(packed_index, sorted_randoms);
#else
            MDHash::MessageBlock input_message(packed_index);
            MDHash::MessageBlock output_message;

            hash.Hash(input_message, output_message);
            
            unsigned long output = output_message;

            if(!Pack(index, output, num_bits))
            {
                cerr << "error in packing" << endl;
                return 1;
            }
#endif
            data.Put(index, output);
        }
        while(counter.Next());
    }
    else if((synthesis_order == GRID_RANDOM) || (synthesis_order == GRID_SCANLINE))
    {  
        int clique_size = Grid::CliqueSize(dimension);
        while(size_per_dim%clique_size) clique_size++;

        // clique_size = 1024; // debug
        
        cerr << "synthesis_order == " << (synthesis_order == GRID_RANDOM ? "GRID_RANDOM" : "GRID_SCANLINE") << " with clique_size " << clique_size << endl;

        if((size_per_dim%clique_size) != 0)
        {
            cerr << "(size_per_dim%clique_size) != 0" << endl;
            return 1;
        }
        
        RandomCounter random_phase_counter(dimension, 0, clique_size-1);
        SequentialCounter sequential_phase_counter(dimension, 0, clique_size-1);

        Counter * random_phase_counter_pointer = &random_phase_counter;
        Counter * sequential_phase_counter_pointer = &sequential_phase_counter;
        Counter & phase_counter = (synthesis_order == GRID_RANDOM ? *random_phase_counter_pointer : *sequential_phase_counter_pointer);

        phase_counter.Reset();

        SequentialCounter sequential_counter(dimension, 0, size_per_dim-1);
        sequential_counter.Reset();

        vector<int> phase_index;
        do
        {
            phase_counter.Get(phase_index);
            
            RandomCounter cell_counter(dimension, 0, (size_per_dim/clique_size)-1);

            cell_counter.Reset();
            vector<int> index;
            vector<int> cell_index;
            
            do
            {
                cell_counter.Get(cell_index);
            
                index = cell_index;

                if((index.size() != cell_index.size()) || (index.size() != phase_index.size()))
                {
                    cerr << "size mismatch" << endl;
                    return 1;
                }
                
                for(unsigned int k = 0; k < index.size(); k++)
                {
                    index[k] = cell_index[k]*clique_size + phase_index[k];
                }

                unsigned long packed_index = 0;

                if(!Pack(index, packed_index, num_bits))
                {
                    cerr << "error in packing" << endl;
                    return 1;
                }
            
#ifdef _SORTED_RANDOM
                const unsigned long output = Query(packed_index, sorted_randoms);
#else
                MDHash::MessageBlock input_message(packed_index);
                MDHash::MessageBlock output_message;

                hash.Hash(input_message, output_message);
            
                unsigned long output = output_message;

                if(!Pack(index, output, num_bits))
                {
                    cerr << "error in packing" << endl;
                    return 1;
                }
#endif
                sequential_counter.Get(index);
                sequential_counter.Next();
                
                data.Put(index, output);
            }
            while(cell_counter.Next());
        }
        while(phase_counter.Next());
    }
    else
    {
        cerr << "unknown synthesis order" << endl;
        return 1;
    }
    
    // output
    ofstream output(output_file_name, ios::binary);

    if(! output)
    {
        cerr << "cannot write into " << output_file_name << endl;
        return 1;
    }

    {
        vector<unsigned long> raw_data;
        data.Get(raw_data);

        for(unsigned int i = 0; i < raw_data.size(); i++)
        {
            const unsigned long foo = raw_data[i]; // - raw_data[(i+1)%raw_data.size()]; // difference as suggested by Zhouchen; doesn't seem to work better.

            // cout << hex << raw_data[i] << " - " << raw_data[(i+1)%raw_data.size()] << " == " << foo << endl;
            // cout << hex << raw_data[i] << endl;
            
            output.write(reinterpret_cast<const char *>(&foo), sizeof(foo));
        }
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
        cerr << "Error: " << e.Message() << endl;
        return 1;
    }
}
