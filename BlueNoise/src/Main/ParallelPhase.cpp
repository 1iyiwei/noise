/*
  ParallelPhase.cpp

  test driver for various phase partition algorithms for parallel generation

  Li-Yi Wei
  07/10/2007

*/

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <time.h>

#include "PhaseRandomCounter.hpp"
#include "FrameBuffer.hpp"
#include "Random.hpp"

int main(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage " << argv[0] << " dimension size clique_size output_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int grid_size = atoi(argv[++argCtr]);
    const int clique_size = atoi(argv[++argCtr]);
    const char * output_file_name = argv[++argCtr];

    // init
    Random::InitRandomNumberGenerator(time(0));
    
    cerr << "build counter " << endl;
    PhaseRandomCounter counter(dimension, 0, grid_size-1, clique_size);

    cerr << "build output array" << endl;
    vector<int> array_size(dimension);
    {
        for(int i = 0; i < dimension; i++)
        {
            array_size[i] = grid_size;
        }
    }
    Array<FrameBuffer::PF> output(array_size);
    FrameBuffer::PF clear_pixel;
    clear_pixel.r = clear_pixel.g = clear_pixel.b = -1;
    output.Clear(clear_pixel);

    // do the work
    cerr << "reset counter " << endl;
    counter.Reset();

    if(! counter.Check())
    {
        cerr << "consistency check failed" << endl;
        return 1;
    }
    
    cout << "# groups = " << counter.NumGroups() << endl;

    cerr << "fill output" << endl;
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

        FrameBuffer::PF pixel;
        pixel.r = pixel.g = pixel.b = cell_id;
        if(!output.Put(cell_location, pixel))
        {
            cerr << "!output.Put(cell_location, cell_id)" << endl;
            return 1;
        }
    }
    while(counter.Next());

    // write out
    cerr << "write out" << endl;
    if(! FrameBuffer::WritePFM(output, output_file_name))
    {
        cerr << "error in writing " << output_file_name << endl;
        return 1;
    }
    
    // done
    return 0;
}
