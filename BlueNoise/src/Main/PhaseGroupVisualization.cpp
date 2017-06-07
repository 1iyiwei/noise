/*
  visualize different phase groups via 2D images

  Li-Yi Wei
  11/24/2007

*/

#include <iostream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "SequentialCounter.hpp"
#include "PhaseRandomCounter.hpp"
#include "RandomCounter.hpp"
#include "FrameBuffer.hpp"
#include "Random.hpp"
#include "Exception.hpp"

//#define _PAPER_IMAGE

int main(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage " << argv[0] << " grid_size clique_size method (0 for grid-partition-grid-order, 1 for random-partition, 2 for grid-partition-random-order) output_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    
    const int grid_size = atoi(argv[++argCtr]);
    const int clique_size = atoi(argv[++argCtr]);
    const int method = atoi(argv[++argCtr]);
    const char * output_file_name = argv[++argCtr];

    const int dimension = 2;
    
    Array<int> grid(vector<int>(dimension, grid_size));
    grid.Clear(-1);
    
    Random::InitRandomNumberGenerator(time(0));

    int num_groups = 0;
    
    if(method == 0)
    {
        cerr << "grid-partition-grid-order" << endl;

        SequentialCounter counter(dimension, 0, grid_size-1);
        counter.Reset();
        
        vector<int> cell_location;
        do
        {
            if(!counter.Get(cell_location))
            {
                cerr << "! counter.Get(cell_location)" << endl;
                return 1;
            }

            int group_id = 0;
            for(unsigned int i = 0; i < cell_location.size(); i++)
            {
                group_id *= clique_size;
                group_id += cell_location[i]%clique_size;
            }

            if(!grid.Put(cell_location, group_id))
            {
                cerr << "!grid.Put(cell_location, group_id)" << endl;
                return 1;
            }
        }
        while(counter.Next());

        num_groups = 1;
        for(unsigned int i = 0; i < dimension; i++)
        {
            num_groups *= clique_size;
        }
    }
    else if(method == 1)
    {
        cerr << "random-partition" << endl;
        PhaseRandomCounter counter(dimension, 0, grid_size-1, clique_size);
        counter.Reset();
        
        num_groups = counter.NumGroups();
            
        vector<int> cell_location;
        int cell_id;
        do
        {
            if(!counter.Get(cell_location))
            {
                cerr << "! counter.Get(cell_location)" << endl;
                return 1;
            }

            if(!counter.GetID(cell_id))
            {
                cerr << "! counter.Get(cell_id)" << endl;
                return 1;
            }

            if(!grid.Put(cell_location, cell_id))
            {
                cerr << "!grid.Put(cell_location, cell_id)" << endl;
                return 1;
            }
        }
        while(counter.Next());
    }
    else if(method == 2)
    {
        cerr << "grid-partition-random-order" << endl;

        vector<int> random_palette;
        {
            RandomCounter counter(dimension, 0, clique_size-1);

            counter.Reset();

            vector<int> index;

            do
            {
                if(!counter.Get(index))
                {
                    cerr << "! counter.Get(index)" << endl;
                    return 1;
                }

                int value = 0;
                for(unsigned int i = 0; i < index.size(); i++)
                {
                    value *= clique_size;
                    value += index[i];
                }
                
                random_palette.push_back(value);
            }
            while(counter.Next());
        }
        
#ifdef _PAPER_IMAGE
        if(random_palette.size() != 9)
        {
            cerr << "random_palette.size() != 9" << endl;
            return 1;
        }
        else
        {
            random_palette[0] = 5;
            random_palette[1] = 0;
            random_palette[2] = 7;
            random_palette[3] = 8;
            random_palette[4] = 4;
            random_palette[5] = 6;
            random_palette[6] = 1;
            random_palette[7] = 3;
            random_palette[8] = 2;
        }
#endif
        
        SequentialCounter counter(dimension, 0, grid_size-1);
        counter.Reset();
        
        vector<int> cell_location;
        do
        {
            if(!counter.Get(cell_location))
            {
                cerr << "! counter.Get(cell_location)" << endl;
                return 1;
            }
            
            int group_id = 0;
            for(unsigned int i = 0; i < cell_location.size(); i++)
            {
                group_id *= clique_size;
                group_id += cell_location[i]%clique_size;
            }

            if((group_id < 0) || (group_id >= random_palette.size()))
            {
                cerr << "(group_id < 0) || (group_id >= random_palette.size())" << endl;
                return 1;
            }
            
            if(!grid.Put(cell_location, random_palette[group_id]))
            {
                cerr << "!grid.Put(cell_location, group_id)" << endl;
                return 1;
            }
        }
        while(counter.Next());
        
        num_groups = 1;
        for(unsigned int i = 0; i < dimension; i++)
        {
            num_groups *= clique_size;
        }
    }
    else
    {
        cerr << "unknown method" << endl;
        return 1;
    }

    // making palette
    if(num_groups <= 0)
    {
        cerr << "num groups <= 0" << endl;
        return 1;
    }
    else
    {
        cerr << "num groups " << num_groups << endl;
    }
    
    vector<FrameBuffer::P3> palette(num_groups);
    for(unsigned int i = 0; i < palette.size(); i++)
    {
        palette[i].r = static_cast<int>(floor(Random::UniformRandom()*255+0.5));
        palette[i].g = static_cast<int>(floor(Random::UniformRandom()*255+0.5));
        palette[i].b = static_cast<int>(floor(Random::UniformRandom()*255+0.5));
    }

#ifdef _PAPER_IMAGE
    {
        vector<FrameBuffer::P3> palet(12);
        palet[0].r = 0xcc; palet[0].g = 0xff; palet[0].b = 0xff;
        palet[1].r = 0xff; palet[1].g = 0xcc; palet[1].b = 0xff;
        palet[2].r = 0xff; palet[2].g = 0xff; palet[2].b = 0xcc;
        palet[3].r = 0xff; palet[3].g = 0x88; palet[3].b = 0xcc;
        palet[4].r = 0xff; palet[4].g = 0xa0; palet[4].b = 0xa0;
        palet[5].r = 0xcc; palet[5].g = 0xcc; palet[5].b = 0xff;
        palet[6].r = 0x00; palet[6].g = 0xff; palet[6].b = 0xff;
        palet[7].r = 0xff; palet[7].g = 0x00; palet[7].b = 0xff;
        palet[8].r = 0xff; palet[8].g = 0xff; palet[8].b = 0x00;
        palet[9].r = 0x00; palet[9].g = 0xff; palet[9].b = 0x00;
        palet[10].r = 0xff; palet[10].g = 0x00; palet[10].b = 0x00;
        palet[11].r = 0x00; palet[11].g = 0x00; palet[11].b = 0xff;

        for(unsigned int i = 0; i < min(palette.size(), palet.size()); i++)
        {
            palette[i] = palet[i];
        }
    }
#endif
    
    // fill output
    Array<FrameBuffer::P3> output(vector<int>(dimension, grid_size));
    {
        vector<int> input_pixels;
        grid.Get(input_pixels);

        vector<FrameBuffer::P3> output_pixels;
        output.Get(output_pixels);

        if(input_pixels.size() != output_pixels.size())
        {
            cerr << "input_pixels.size() != output_pixels.size()" << endl;
            return 1;
        }

        for(unsigned int i = 0; i < output_pixels.size(); i++)
        {
            const int selection = input_pixels[i];

            if((selection < 0) || (selection >= palette.size()))
            {
                cerr << "selection outside palette" << endl;
                return 1;
            }

            output_pixels[i] = palette[selection];
        }
        
        output.Put(output_pixels);
    }

    // write
    if(! FrameBuffer::WritePPM(output, 255, output_file_name))
    {
        cerr << "cannot write to " << output_file_name << endl;
        return 1;
    }

    // done
    return 0;
}
