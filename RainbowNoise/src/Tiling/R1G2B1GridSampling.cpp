/*
  R1G2B1GridSampling.cpp

  the usual grid camera sensor color filter layout

  Li-Yi Wei
  10/16/2008

*/

#include <iostream>
using namespace std;

#include <stdlib.h>

#include "Exception.hpp"

int Main(int argc, char **argv)
{
    int min_argc = 5;

    if(argc < min_argc)
    {
        cerr << "Usage: " << argv[0] << " domain_width_float domain_height_float grid_width_int grid_height_int" << endl;
        return 1;
    }

    int argCtr = 0;
    
    const float domain_width = atof(argv[++argCtr]);
    const float domain_height = atof(argv[++argCtr]);
    const int grid_width = atoi(argv[++argCtr]);
    const int grid_height = atoi(argv[++argCtr]);

    const float cell_width = domain_width/grid_width;
    const float cell_height = domain_height/grid_height;

    for(int i = 0; i < grid_width; i++)
        for(int j = 0; j < grid_height; j++)
        { 
            // from Penrose pixels paper, Figure 1 (a)
            // 0: red, 1: green, 2: blue
            const int class_id = 0*((i%2 == 1) && (j%2 == 0)) + 1*(((i%2) + (j%2)) != 1) + 2*((i%2 == 0) && (j%2 == 1));

            const float x = (i+0.5)*cell_width;
            const float y = (j+0.5)*cell_height;

            cout << class_id << " " << x << " " << y << endl;
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
