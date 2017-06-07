/*
  Check minimum distance among samples

  Li-Yi Wei
  12/01/2007

*/



#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "Exception.hpp"
#include "SFT.hpp"

float Distance2(const Sample & s1, const Sample s2)
{
    if(s1.coordinate.Dimension() != s2.coordinate.Dimension())
    {
        throw Exception("incompatible dimensions");
    }

    float result = 0;

    for(int i = 0; i < s1.coordinate.Dimension(); i++)
    {
        const float diff = s1.coordinate[i] - s2.coordinate[i];
        result += diff*diff;
    }

    return result;
}

int Main(int argc, char **argv)
{
    if(argc < 5)
    {
        cerr << "Usage: " << argv[0] << " samples-file-name dimension r boundary (0 for none, 1 for toroidal)" << endl;
        return 1;
    }

    int argCtr = 0;
    const char * input_file_name = argv[++argCtr];
    const int dimension = atoi(argv[++argCtr]);
    const float min_distance = atof(argv[++argCtr]);
    const int boundary = atoi(argv[++argCtr]);

    const float error_tolerance = 1.0;
    
    ifstream input(input_file_name); // read in samples
    cout << "read in samples " << endl;
    
    vector<Sample> samples;
    
    if(!input)
    {
        return 0;
    }
    else
    {
        Sample sample(dimension);
        
        while(input.good())
        {
            for(int i = 0; i < dimension; i++)
            {
                input >> sample.coordinate[i];
            }
            
            if(input.good())
            {
                samples.push_back(sample);
            }
        }
    }

    cout << "total " << samples.size() << " samples read" << endl;

    // check
    const float min_distance2 = min_distance*min_distance;

    float current_min2 = min_distance2;
    int min_i = 0; int min_j = 0;
    for(unsigned int i = 0; i < samples.size(); i++)
        for(unsigned int j = i+1; j < samples.size(); j++)
        {
            const float distance2 = Distance2(samples[i], samples[j]);

            if(distance2 < current_min2)
            {
                current_min2 = distance2;
                min_i = i; min_j = j;
            }
        }
    
    // done
    if(current_min2 < min_distance2)
    {
        cerr << "distance check failed: dist (";
        for(unsigned int k = 0; k < samples[min_i].coordinate.Dimension(); k++)
        {
            cerr << samples[min_i].coordinate[k];
            if(k < (samples[min_i].coordinate.Dimension()-1)) cerr << ", ";
        }
        cerr << ") (";
        for(unsigned int k = 0; k < samples[min_j].coordinate.Dimension(); k++)
        {
            cerr << samples[min_j].coordinate[k];
            if(k < (samples[min_j].coordinate.Dimension()-1)) cerr << ", ";
        }
        cerr << ") == " << sqrt(current_min2) << " < " << min_distance << endl;
        return 0;
    }
    else
    {
        cerr << "distance check passed" << endl;
        return 0;
    }
}

int main(int argc, char **argv)
{
    try
    {
        return Main(argc, argv);
    }
    catch(Exception e)
    {
        cerr<<"Error : "<<e.Message()<<endl;
        return 1;
    }
}
