/*
  Blueness.cpp

  measure how close a distribution is to an ideal blue noise

  Li-Yi Wei
  11/05/2010

*/

#include <iostream>
#include <string>
using namespace std;

#include <stdlib.h>
#include <math.h>

#include "DiscreteDiffDistribution.hpp"
#include "BlueNoiseDiffDistribution.hpp"
#include "FrameBuffer.hpp"
#include "Math.hpp"
#include "Utility.hpp"
#include "SequentialCounter.hpp"
#include "Exception.hpp"

int Main(int argc, char **argv)
{
    // input arguments
    int argc_min = 6;

    if(argc < argc_min)
    {
        cerr << argv[0] << " input_file_name cell_size diff_domain_max (dimension numbers) core_radius core_weight_scale" << endl;
        return 1;
    }

    int arg_ctr = 0;
    
    const string input_file_name = argv[++arg_ctr];
    const float cell_size = atof(argv[++arg_ctr]);

    Array<float> input_raw;
    if(! FrameBuffer::ReadF2M(input_file_name, input_raw))
    {
        cerr << "cannot read from " << input_file_name << endl;
        return 1;
    }

    const int dimension = input_raw.Dimension();

    if(argc < (argc_min + dimension - 1))
    {
        cerr << "not enough input arguments" << endl;
        return 1;
    }
    
    vector<float> diff_domain_min(dimension);
    for(unsigned int i = 0; i < diff_domain_min.size(); i++)
    {
        diff_domain_min[i] = -atof(argv[++arg_ctr]);
    }

    vector<float> domain_size(dimension);
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        domain_size[i] = cell_size*input_raw.Size(i);
    }

    vector<float> diff_domain_max(dimension);
    for(unsigned int i = 0; i < diff_domain_max.size(); i++)
    {
        diff_domain_max[i] = diff_domain_min[i] + domain_size[i];
    }

    const float radius = atof(argv[++arg_ctr]);
    const float core_weight_scale = atof(argv[++arg_ctr]);

    // distributions
    string message = Utility::Normalize(input_raw, domain_size);
    if(message != "")
    {
        cerr << "cannot normalize input: " << message << endl;
        return 1;
    } 

    DiscreteDiffDistribution input_distribution(input_raw, cell_size, diff_domain_min);

    BlueNoiseDiffDistribution blue_distribution(diff_domain_min, diff_domain_max, radius, 1.0);

    // relative weight
    const float core_volume = Math::HyperSphereVolume(dimension, radius);
    float total_volume = 1;
    for(unsigned int i = 0; i < domain_size.size(); i++)
    {
        total_volume *= domain_size[i];
    }

    const float scaled_core_volume = core_volume * core_weight_scale;
    const float core_weight = 0.5*total_volume/scaled_core_volume;
    const float rest_weight = 0.5*total_volume/(total_volume - scaled_core_volume);

    // difference
    vector<int> input_size;
    input_raw.Size(input_size);

    SequentialCounter counter(dimension, vector<int>(dimension, 0), Utility::Minus1(input_size));
    counter.Reset();

    vector<int> index(dimension);
    DiffDistribution::Diff diff;
    diff.diff = Coordinate(dimension);

    float sum = 0;
    do
    {
        counter.Get(index);
        
        for(int i = 0; i < diff.diff.Dimension(); i++)
        {
            diff.diff[i] = diff_domain_min[i] + cell_size*index[i];
        }

        const float input_value = input_distribution.Query(diff);
        const float blue_value = blue_distribution.Query(diff);

        const float weight = (Utility::Norm2(diff.diff) < radius*radius ? core_weight : rest_weight);

        sum += weight*fabs(input_value - blue_value);
    }
    while(counter.Next());

    const float cell_volume = pow(cell_size*1.0, dimension*1.0);
    sum *= cell_volume;

    // report
    cout << "non-blue-ness for " << input_file_name << " with core weight scale " << core_weight_scale << " : " << sum << endl;

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
