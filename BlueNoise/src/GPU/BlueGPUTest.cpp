/*
  BlueGPU.cpp

  the main driver for GPU Poisson disk sampling (Blue noise) generation

  Li-Yi Wei
  11/23/2007

*/

#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "Random.hpp"

#include "CgBlue.hpp"

int Test(int argc, char **argv)
{
    const int num_fixed_arguments = 4;
    
    if(argc < num_fixed_arguments)
    {
        cerr << "Usage: " << argv[0] << " dimension texture_size (dimension numbers) clique_size num_tests" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);

    if(argc < num_fixed_arguments+dimension)
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }
    
    vector<int> texture_size_input(dimension);

    for(unsigned int i = 0; i < texture_size_input.size(); i++)
    {
        texture_size_input[i] = atoi(argv[++argCtr]);
    }

    const CgBlue::TextureSize texture_size(texture_size_input);

    const int clique_size = atoi(argv[++argCtr]);

    const int num_tests = atoi(argv[++argCtr]);
    
    CgBlue::TextureSize texture_phase_group_size;
    if(! CgBlue::ComputePhaseGroupSize(texture_size, clique_size, texture_phase_group_size))
    {
        cerr << "error in ComputePhaseGroupSize" << endl;
        return 1;
    }

    //const vector<int> phase_group_size(texture_phase_group_size);
    const CgBlue::TextureSize phase_group_size(texture_phase_group_size);
    
    vector<int> input(texture_size.Dimension());
    vector<int> output(2);
    vector<int> recovered(texture_size.Dimension());

    Random::InitRandomNumberGenerator();
    for(int trial = 0; trial < num_tests; trial++)
    {
        for(unsigned int i = 0; i < input.size(); i++)
        {
            input[i] = static_cast<int>(floor(Random::UniformRandom()*texture_size_input[i]));
            if(input[i] >= texture_size_input[i]) input[i] = (texture_size_input[i]-1);
            if(input[i] < 0) input[i] = 0;
        }

        if(! CgBlue::PackIntegerCoordinate(phase_group_size, clique_size, input, output))
        {
            cerr << "error in PackIntegerCoordinate" << endl;
            return 1;
        }

        if(texture_size_input.size() != output.size())
        {
            cerr << "texture_size_input.size() != output.size()" << endl;
            return 1;
        }
        else
        {
            for(unsigned int i = 0; i < output.size(); i++)
            {
                if((output[i] < 0) || (output[i] >= texture_size_input[i]))
                {
                    cerr << "illegal range from PackIntegerCoordinate" << endl;
                    return 1;
                }
            }
        }
        
        if(! CgBlue::UnPackIntegerCoordinate(phase_group_size, clique_size, output, recovered))
        {
            cerr << "error in UnPackIntegerCoordinate" << endl;
            return 1;
        }
        
        if(input != recovered)
        {
            cerr << "PackIntegerCoordinate(): input(";
            for(unsigned int i = 0; i < input.size(); i++)
            {
                cerr << input[i];
                if(i < (input.size()-1)) cerr << " ";
            }
            cerr << ") " << (input != recovered ? "!" : "=") << "= recovered(";
            for(unsigned int i = 0; i < recovered.size(); i++)
            {
                cerr << recovered[i];
                if(i < (recovered.size()-1)) cerr << " ";
            }
            cerr << ") - output(";
            for(unsigned int i = 0; i < output.size(); i++)
            {
                cerr << output[i];
                if(i < (output.size()-1)) cerr << " ";
            }
            cerr << ")" << endl;
            if(input != recovered) return 1;
        }
        
        if(! CgBlue::PackFlattenIntegerCoordinate(texture_size, phase_group_size, clique_size, input, output))
        {
            cerr << "error in PackFlattenIntegerCoordinate" << endl;
            return 1;
        }

        if(output.size() != 2)
        {
            cerr << "illegal output size" << endl;
            return 1;
        }
        else
        {
            if((output[0] < 0) || (output[0] >= texture_size.TotalWidth()) || (output[1] < 0) || (output[1] >= texture_size.TotalHeight()))
            {
                cerr << "illegal range from PackFlattenIntegerCoordinate" << endl;
                return 1;
            }
        }
        
        if(! CgBlue::UnFlattenUnPackIntegerCoordinate(texture_size, phase_group_size, clique_size, output, recovered))
        {
            cerr << "error in UnFlattenUnPackIntegerCoordinate" << endl;
            return 1;
        }

        if(input != recovered)
        {
            cerr << "PackFlattenIntegerCoordinate(): input(";
            for(unsigned int i = 0; i < input.size(); i++)
            {
                cerr << input[i];
                if(i < (input.size()-1)) cerr << " ";
            }
            cerr << ") " << (input != recovered ? "!" : "=") << "= recovered(";
            for(unsigned int i = 0; i < recovered.size(); i++)
            {
                cerr << recovered[i];
                if(i < (recovered.size()-1)) cerr << " ";
            }
            cerr << ") - output(";
            for(unsigned int i = 0; i < output.size(); i++)
            {
                cerr << output[i];
                if(i < (output.size()-1)) cerr << " ";
            }
            cerr << ")" << endl;
            if(input != recovered) return 1;
        }
    }
    
    cerr << "all tests passed" << endl;
    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return Test(argc, argv);
    }
    catch(Exception e)
    {
        cerr << "Error : "<< e.Message() << endl;
        return 1;
    }
}
