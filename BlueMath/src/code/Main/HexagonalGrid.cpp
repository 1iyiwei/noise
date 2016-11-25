/*
  HexagonalGrid.cpp

  generate hexagonal grid samples

  Li-Yi Wei
  11/24/2010

*/

#include <iostream>
using namespace std;

#include "Exception.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <memory>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "SequentialCounter.hpp"
#include "FrameBuffer.hpp"
#include "Utility.hpp"
#include "Math.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 4;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " dimension domain_size (dimension numbers) sample_rate_per_unit_domain" << endl;
        return 1;
    }

    int arg_ctr = 0;
    
    const int dimension = atoi(argv[++arg_ctr]);

    if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension, 1.0);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }
    
    const int sample_rate = atoi(argv[++arg_ctr]);
    
    const float cell_size = 1.0/sample_rate;

    const float spacing0 = cell_size;
    const float spacing1 = spacing0*cos(30*Math::PI/180);

    const int sample_rate0 = ceil(sample_rate*domain_size[0]);
    const int sample_rate1 = ceil(sample_rate/cos(30*Math::PI/180)*domain_size[1]);

    vector<float> padding(dimension, 0); // ensure the hexagonal samples don't get too close together across boundaries

    // samples
    vector<Sample> samples;
    Sample sample(dimension);
    sample.id = 0;

    for(int row = 0; row < sample_rate0; row++)
    {
        for(int col = 0; col < sample_rate1; col++)
        {
            padding[0] = (row > 0)*spacing0*(1-(col%2)*0.5);
            padding[1] = spacing1;

            sample.coordinate[0] = ((col%2)*0.5 + row)*spacing0;
            sample.coordinate[1] = spacing1*col;

            int inside = 1;
            for(int i = 0; i < sample.coordinate.Dimension(); i++)
            {
                if((sample.coordinate[i] >= (domain_size[i]-padding[i])))
                {
                    inside = 0;
                }
            }

            if(inside)
            {
                samples.push_back(sample);
            }
        }
    }

    // output
    // cerr << samples.size() << " samples" << endl;
    if(!Utility::WriteSamples(samples, "cout"))
    {
        cerr << "cannot write out samples" << endl;
        return 1;
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
