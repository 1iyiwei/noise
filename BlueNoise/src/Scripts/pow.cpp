#include <iostream>
using namespace std;

#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << "base power" << endl;
        return 1;
    }

    int argCtr = 0;

    const float x = atof(argv[++argCtr]);
    const float y = atof(argv[++argCtr]);

    cout << x << "^" << y << " = " << pow(x, y) << endl;

    // done
    return 1;
}
