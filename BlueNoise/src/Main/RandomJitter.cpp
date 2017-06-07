/*
  Random jittering from a regular grid

  Li-Yi Wei
  06/18/2007

*/

#include <iostream>
#include <vector>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

void INIT_RANDOM_NUMBER_GENERATOR(void)
{
    srand48(time(NULL));
}

double UNIFORM_RANDOM(void)
{
    return drand48();
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " grid-size jitter-scale (between [0 1])" << endl;
        return 1;
    }

    const int grid_size = atoi(argv[1]);
    const float jitter_scale = atof(argv[2]);

    const float grid_length = 1.0/grid_size;

    INIT_RANDOM_NUMBER_GENERATOR();

    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
        {
            cout << grid_length*(i+0.5+jitter_scale*(UNIFORM_RANDOM()-0.5)) << " " << grid_length*(j+0.5+jitter_scale*(UNIFORM_RANDOM()-0.5)) << endl;
        }

    return 0;
}
