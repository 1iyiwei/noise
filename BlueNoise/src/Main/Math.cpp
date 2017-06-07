/*
  Math.cpp

  driver for statistical measurements

  Li-Yi Wei
  08/16/2007

*/

#include <iostream>
using namespace std;
#include <stdlib.h>

#include "Exception.hpp"
#include "Math.hpp"

int MaxMinDistance(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " dimension num_samples" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float num_samples = atof(argv[++argCtr]);

    cout << "MaxMinDistance(" << dimension << ", " << num_samples << ") == " << Math::ComputeMaxMinDistance(dimension, num_samples) << endl;

    return 0;
}
    
int main(int argc, char **argv)
{
    try
    {
        return MaxMinDistance(argc, argv);
    }
    catch(Exception e)
    {
        cerr << e.Message() << endl;
        return 1;
    }
}
