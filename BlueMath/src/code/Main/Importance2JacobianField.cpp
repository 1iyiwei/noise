/*
  Importance2JacobianField.cpp

  convert a scalar importance field into a Jacobian field

  Li-Yi Wei
  10/28/2010

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

#include "ImportanceJacobianField.hpp"

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
        cerr << "Usage: " << argv[0] << " input_file_name max_importance cell_size r_value output_file_name" << endl;
        return 1;
    }

    int arg_ctr = 0;
    const string input_file_name = argv[++arg_ctr];
    const float max_importance = atof(argv[++arg_ctr]);
    const float cell_size = atof(argv[++arg_ctr]);
    const float r_value = atof(argv[++arg_ctr]);
    const string output_file_name = argv[++arg_ctr];

    // importance
    Array<float> importance;
    if(! FrameBuffer::ReadF2M(input_file_name, importance))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    const int dimension = importance.Dimension();

    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = importance.Size(i)*cell_size;
    }

    PlainBoxDomain domain(domain_size);

    // Jacobian
    Array<Matrix> inverse_jacobians;

    if(! ImportanceJacobianField::Convert(domain, importance, max_importance, cell_size, r_value, inverse_jacobians))
    {
        cerr << "error in converting importance into Jacobian" << endl;
        return 1;
    }

    // output
    if(! Utility::WriteMatrixArray(inverse_jacobians, output_file_name))
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
