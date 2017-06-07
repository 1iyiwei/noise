/*
  mean_divide.cpp

  convert old raial mean data into new one

  Li-Yi Wei
  12/27/2007

*/

#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " input_file_name num_samples" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const float num_samples = atof(argv[++argCtr]);
    
    ifstream input(input_file_name);

    if(! input)
    {
        cerr << "cannot read " << input_file_name << endl;
        return 1;
    }

    while(input.good())
    {
        double value;
        input >> value;
        if(input.good())
        {
            cout << value/(num_samples*num_samples) << " ";
        }
    }

    // done
    return 0;
}
