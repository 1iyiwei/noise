/*
  kstest.cpp

  Li-Yi Wei
  11/05/2007

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

#include <math.h>
#include <stdlib.h>

typedef double real;

real AD(const real z)
{
    if( z<.01 ) return 0;

    if( z<=2 )
    {
        return 2*exp(-1.2337/z)*(1+z/8-.04958*z*z/(1.325+z))/sqrt(z);
    }

    if( z<=4 ) return 1-.6621361*exp(-1.091638*z)-.95095*exp(-2.005138*z); 

    if( 4<z ) return 1-.4938691*exp(-1.050321*z)-.5946335*exp(-1.527198*z);

    return -1; 
}

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        cerr << "Usage: " << argv[0] << " input_file_name" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];

    ifstream input(input_file_name);

    vector<real> numbers;

    if(!input)
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    while(input.good())
    {
        real value;
        input >> value;

        if(input.good())
        {
            numbers.push_back(value);
        }
    }

    sort(numbers.begin(), numbers.end());

    // d-value
    real d_value = 0;
    for(unsigned int i = 0; i < numbers.size(); i++)
    {
        const real current_d = fabs(numbers[i] - (i+1)*1.0/numbers.size());
        if(current_d > d_value) d_value = current_d;
    }

    // p-value
    const real epsilon = pow(10.0, -20.0);
    real z_value = -1.0*numbers.size()*numbers.size();
    
    for(unsigned int i = 0; i < numbers.size(); i++)
    {
        real tmp = numbers[i]*(1 - numbers[numbers.size()-1-i]);
        tmp = max(epsilon, tmp);
        z_value -= (2*i+1)*log(tmp);
    }

    z_value /= numbers.size();
    const real p_value = 1 - AD(z_value);

    // the d-value obtained here is the same as the k (3rd argument) from kstest under Matlab
    // the p-value obtained here is 1-p_value from kstest.c under the old DIEHARD source code, but it is equivalent to the results produced by compiling the sources myself.
    // the 
    cout << "d-value: " << d_value << ", p-value: " << p_value << endl;
    return 0;
}
