/*
  UniformNDim.cpp

  Li-Yi Wei
  07/15/2007

*/

#if 0
#define _UNIFORM_DISTANCE_FIELD

#include "AdaptiveNDim.cpp"
#else

#include <iostream>
using namespace std;

int main(int argc, char **argv)
{
    cerr << argv[0] << " is deprecated. use AdaptiveNDim with 'uniform' option" << endl;
    return 1;
}

#endif
