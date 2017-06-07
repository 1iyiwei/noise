/*
  DartThrowing.cpp

  brute force dart throwing
  toroidal boundary condition not yet handled
  appears to be very slow
  
  Li-Yi Wei
  12/07/2007

*/

#include <iostream>
#include <vector>
#include <deque>
using namespace std;

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "Exception.hpp"
#include "Random.hpp"
#include "Sample.hpp"

float Distance2(const Sample & s1, const Sample & s2)
{
    if(s1.coordinate.Dimension() != s2.coordinate.Dimension())
    {
        throw Exception("s1.coordinate.Dimension() != s2.coordinate.Dimension()");
    }
    
    float result = 0;

    for(unsigned int i = 0; i < s1.coordinate.Dimension(); i++)
    {
        const float foo = s1.coordinate[i] - s2.coordinate[i];
        result += foo*foo;
    }

    return result;
}

int Main(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " dimension r_number k_number num_samples" << endl;
        return 1;
    }
    
    int argCtr = 0;
    const int dimension = atoi(argv[++argCtr]);
    const float min_distance = atof(argv[++argCtr]);
    const int k_number = atoi(argv[++argCtr]);
    const int num_samples = atoi(argv[++argCtr]);

    Random::InitRandomNumberGenerator();
    
    vector<Sample> samples;

    const float min_distance2 = min_distance*min_distance;

    int num_failures = 0;
    
    while((samples.size() < num_samples) && ((k_number <= 0) || (num_failures < k_number)))
    {
        Sample baby(dimension);

        for(unsigned int i = 0; i < baby.coordinate.Dimension(); i++)
        {
            baby.coordinate[i] = Random::UniformRandom();
        }

        int legal = 1;
        
        for(unsigned int i = 0; i < samples.size(); i++)
        {
            const float distance2 = Distance2(samples[i], baby);

            if(distance2 < min_distance2)
            {
                legal = 0;
                break;
            }
        }

        if(legal)
        {
            samples.push_back(baby);
            num_failures = 0;
        }
        else
        {
            num_failures++;
        }
    }

    // output stuff
    cerr << "total " << samples.size() << " samples generated" << endl;
    for(unsigned int i = 0; i < samples.size(); i++)
    {
        for(int j = 0; j < samples[i].coordinate.Dimension(); j++)
        {
            cout << samples[i].coordinate[j] << " ";
        }
        cout << endl;
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
