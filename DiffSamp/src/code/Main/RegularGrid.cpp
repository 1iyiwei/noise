/*
  RegularGrid.cpp

  generate regular grid samples

  Li-Yi Wei
  09/06/2010

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
#include "Random.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    const int argc_min = 5;

    if(argc < argc_min)
    {
        cerr << "Usage: " << argv[0] << " dimension domain_size (dimension numbers) sample_rate (dimension integers) jitter_scale (between 0 and 1)" << endl;
        return 1;
    }

    int arg_ctr = 0;
    
    const int dimension = atoi(argv[++arg_ctr]);

    if(argc < (argc_min + 2*(dimension - 1)))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }

    vector<float> domain_size(dimension, 1.0);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = atof(argv[++arg_ctr]);
    }
    
    vector<int> sample_rate(dimension, 1);
    for(unsigned int i = 0; i < sample_rate.size(); i++)
    {
        sample_rate[i] = atoi(argv[++arg_ctr]);
    }
    
    const float jitter_scale = atof(argv[++arg_ctr]);

    vector<float> cell_size(domain_size);
    for(unsigned int i = 0; i < cell_size.size(); i++)
    {
        cell_size[i] = domain_size[i]/sample_rate[i];
    }

    vector<int> counter_max(sample_rate);
    for(unsigned int i = 0; i < counter_max.size(); i++)
    {
        counter_max[i] = sample_rate[i]-1;
    }

    SequentialCounter counter(dimension, vector<int>(dimension, 0), counter_max);

    // random init
    Random::InitRandomNumberGenerator();

    // samples
    vector<Sample> samples;
    Sample sample(dimension);
    sample.id = 0;

    vector<int> index;
    counter.Reset();

    do
    {
        counter.Get(index);

        for(int i = 0; i < sample.coordinate.Dimension(); i++)
        {
            sample.coordinate[i] = (index[i] + 0.5 + jitter_scale*(Random::UniformRandom() - 0.5))*cell_size[i];
        }

        samples.push_back(sample);
    }
    while(counter.Next());

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
