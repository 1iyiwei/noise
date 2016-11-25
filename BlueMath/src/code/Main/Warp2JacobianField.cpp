/*
  Warp2JacobianField.cpp

  sample a warp into a jacobian field

  Li-Yi Wei
  10/21/2008

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <memory>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "PlainBoxDomain.hpp"

#include "MatrixWarp.hpp"
#include "TerrainWarp.hpp"

#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "SequentialCounter.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 6;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " warp_option dimension domain_size (dimension numbers) cell_size output_file_name" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string warp_option = argv[++arg_ctr];
    
    const int dimension = atof(argv[++arg_ctr]);

    if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension, 1.0);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }
    
    const float cell_size = atof(argv[++arg_ctr]);

    const string output_file_name = argv[++arg_ctr];

    Matrix * p_matrix = 0;
    Warp * p_warp = 0;

    if(warp_option == "terrain")
    {
        p_warp = new TerrainWarp;
    }
    else
    {
        // warp matrix
        Array<FrameBuffer::F2> input;
        if(!FrameBuffer::ReadF2M(warp_option, input))
        {
            cerr << "cannot read from " << warp_option << endl;
            return 1;
        }

        p_matrix = new Matrix(input);

        p_warp = new MatrixWarp(*p_matrix);
    }

    if(!p_warp) throw Exception("null warp");

    const Warp & warp = *p_warp;

    PlainBoxDomain domain(domain_size);

    vector<int> resolution(domain_size.size());

    for(unsigned int i = 0; i < resolution.size(); i++)
    {
        resolution[i] = static_cast<int>(floor(domain_size[i]/cell_size + 0.5));
    }
    
    SequentialCounter counter(resolution.size(), vector<int>(resolution.size(), 0), Utility::Minus1(resolution));

    Array<Matrix> output(resolution);

    Sample sample(dimension);
    vector<int> index;
    Matrix jacobian;

    counter.Reset();
    do
    {
        counter.Get(index);

        if(sample.coordinate.Dimension() != index.size())
        {
            sample.coordinate = Coordinate(index.size());
        }

        for(int i = 0; i < sample.coordinate.Dimension(); i++)
        {
            sample.coordinate[i] = cell_size*index[i];
        }

        if(!warp.InverseJacobian(sample.coordinate, jacobian)) throw Exception("cannot get warp inverse jacobian");

        if(!output.Put(index, jacobian)) throw Exception("cannot put output jacobian");
    }
    while(counter.Next());

    if(! Utility::WriteMatrixArray(output, output_file_name))
    {
        cerr << "cannot write to " << output_file_name << endl;
        return 1;
    }

    // clean up
    if(! p_warp)
    {
        delete p_warp;
        p_warp = 0;
    }

    if(! p_matrix)
    {
        delete p_matrix;
        p_matrix = 0;
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
