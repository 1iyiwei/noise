/*
  Matrix.cpp

  a collection of matrix related operations

  Li-Yi Wei
  10/16/2010

*/

#include <iostream>
#include <string>
using namespace std;

#include "Matrix.hpp"
#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Exception.hpp"

int Invert(int argc, char **argv)
{
    // input arguments
    int min_argc = 3;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_matrix output_matrix" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_matrix_name = argv[++arg_ctr];
    const string output_matrix_name = argv[++arg_ctr];

    Array<float> input;
    if(! FrameBuffer::ReadF2M(input_matrix_name, input))
    {
        cerr << "cannot read from " << input_matrix_name << endl;
    }

    Matrix input_matrix(input);
    Matrix output_matrix;

    if(! input_matrix.Inverse(output_matrix))
    {
        cerr << "cannot invert matrix" << endl;
        return 1;
    }

    if(! FrameBuffer::WriteF2M(output_matrix, output_matrix_name))
    {
        cerr << "cannot write to " << output_matrix_name << endl;
        return 1;
    }

    // done
    return 0;
}

int Multiply(int argc, char **argv)
{
    // input arguments
    int min_argc = 4;

    if(argc < min_argc)
    {
        cerr << argv[0] << " input_matrix1 input_matrix2 output_matrix" << endl;
        return 1;
    }
    
    int arg_ctr = 0;
    const string input_matrix1_name = argv[++arg_ctr];
    const string input_matrix2_name = argv[++arg_ctr];
    const string output_matrix_name = argv[++arg_ctr];

    Array<float> input1, input2;
    if(! FrameBuffer::ReadF2M(input_matrix1_name, input1))
    {
        cerr << "cannot read from " << input_matrix1_name << endl;
    }
    if(! FrameBuffer::ReadF2M(input_matrix2_name, input2))
    {
        cerr << "cannot read from " << input_matrix2_name << endl;
    }

    Matrix input_matrix1(input1);
    Matrix input_matrix2(input2);
    Matrix output_matrix;

    if(! input_matrix1.Multiply(input_matrix2, output_matrix))
    {
        cerr << "cannot multiply matrix" << endl;
        return 1;
    }

    if(! FrameBuffer::WriteF2M(output_matrix, output_matrix_name))
    {
        cerr << "cannot write to " << output_matrix_name << endl;
        return 1;
    }

    // done
    return 0;
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

        int arg_ctr = 0;
        const string option = argv[++arg_ctr];

        if(option == "invert")
        {
            return Invert(argc-1, &argv[1]);
        }
        if(option == "multiply")
        {
            return Multiply(argc-1, &argv[1]);
        }
        else
        {
            cerr << "unknown matrix option" << endl;
            return 1;
        }
    }
    catch(Exception e)
    {
        cerr << "Error : " << e.Message() << endl;
        return 1;
    }
}
