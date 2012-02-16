/*
  siblingr.cpp

  compute another r from a brother and mother

  Li-Yi Wei
  05/07/2008

*/

#include <iostream>
using namespace std;

#include <stdlib.h>
#include <math.h>

int main(int argc, char**argv)
{
    if(argc < 4)
    {
        cerr << "Usage: " << argv[0] << " dimension brother-r mother-r" << endl;
        return 1;
    }

    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const double brother_r = atof(argv[++argCtr]);
    const double mother_r = atof(argv[++argCtr]);

    const double my_r = pow(1.0/(1.0/pow(mother_r, 1.0*dimension) - 1.0/pow(brother_r, dimension*1.0)), 1.0/dimension);

    cout << my_r << endl;

    return 0;
}
