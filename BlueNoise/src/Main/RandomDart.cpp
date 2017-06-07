/*
  similar to PoissonDisk.cpp, but use (uniform) random dart throwing

  Li-Yi Wei
  06/16/2007 original code compiled under cygwin
  12/27/2007 integrated into Visual studio system
*/

#include <iostream>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "Random.hpp"

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " dimension number-of-points" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const int number_of_points = atoi(argv[++argCtr]);

    Random::InitRandomNumberGenerator();

    for(int i = 0; i < number_of_points; i++)
    {
        for(int j = 0; j < dimension; j++)
        {
            cout << Random::UniformRandom() << " ";
        }
        cout << endl;
    }

    return 0;
}
