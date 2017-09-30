/*
  TileF2M.cpp

  Li-Yi Wei
  10/11/2008

*/

#include <iostream>
#include <algorithm>
using namespace std;

#include <stdlib.h>

#include "FrameBuffer.hpp"
#include "SequentialCounter.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

template<class T>
int Tile(const Array<T> & input, const vector<int> & num_tiles, Array<T> & output)
{
    if(input.Dimension() != num_tiles.size())
    {
        return 0;
    }

    const int dimension = input.Dimension();

    vector<int> input_size;
    input.Size(input_size);
    
    vector<int> output_size(input_size);

    for(unsigned int i = 0; i < output_size.size(); i++)
    {
        output_size[i] = input_size[i]*num_tiles[i];
    }

    output = Array<T>(output_size);

    vector<int> output_size_minus1 = Utility::Minus1(output_size);

    SequentialCounter counter(dimension, vector<int>(dimension, 0), output_size_minus1);

    counter.Reset();

    vector<int> input_index(dimension);
    vector<int> output_index(dimension);
    T element;

    do
    {
        counter.Get(output_index);
        
        input_index = output_index;
        for(unsigned int i = 0; i < input_index.size(); i++)
        {
            input_index[i] %= input_size[i];
        }
        
        if(! input.Get(input_index, element)) throw Exception("TileF2M: cannot get input!");

        if(! output.Put(output_index, element)) throw Exception("TileF2M: cannot put output!");
    }
    while(counter.Next());

    return 1;
}

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 4;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " input_file_name num_tiles (between 1 to dimension numbers) output_file_name" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];

    Array<FrameBuffer::F2> input;
    if(!FrameBuffer::ReadF2M(input_file_name, input))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    const int dimension = input.Dimension();

    const int tile_dimension = min(dimension, (argc - argc_min + 1));
    const int element_dimension = dimension - tile_dimension;

    if(tile_dimension < 1) // if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<int> num_tiles(tile_dimension);
    for(unsigned int i = 0; i < num_tiles.size(); i++)
    {
        if((arg_ctr + 1) < argc)
        {
            num_tiles[i] = atoi(argv[++arg_ctr]);
        }
        else
        {
            throw Exception("not enough input arguments");
        }
    }

    const string output_file_name = argv[++arg_ctr];

    // process
    Array<FrameBuffer::F2> output;

    if(element_dimension <= 0)
    {
        if(! Tile(input, num_tiles, output))
        {
            cerr << "cannot tile" << endl;
            return 1;
        }
    }
    else
    {
        Array< Array<FrameBuffer::F2> > input2, output2;
    
        if(! Utility::Convert(input, element_dimension, input2))
        {
            cerr << "error in converting input to input2" << endl;
            return 1;
        }

        if(! Tile(input2, num_tiles, output2))
        {
            cerr << "cannot tile" << endl;
            return 1;
        }

        if(! Utility::Convert(output2, output))
        {
            cerr << " error in converting output2 to output" << endl;
            return 1;
        }
    }

    // write out
    if(!FrameBuffer::WriteF2M(output, output_file_name))
    {
        cerr << "cannot write to " << output_file_name << endl;
        return 1;
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
